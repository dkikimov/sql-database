//
// Created by Даник 💪 on 06.05.2023.
//

#include "Parser.h"
#include "../structures/Constants.h"
#include "../db/algorithms.cpp"

#include <stack>

Parser::Parser(Lexer& lexer) : lexer_(lexer) {}

Table Parser::ParseCreateTable() {
  Table result;

  Token token = lexer_.GetNextToken();

  if (token.type != TOKEN_KEYWORD) {
    throw SQLError(SYNTAX_ERROR);
  }

  result.name = token.value;
  token = lexer_.GetNextToken();

  if (token.type != TokenTypes::TOKEN_LBRACE) {
    throw SQLError(SYNTAX_ERROR);
  }
  result.columns = ParseCreateTableColumns();
  return result;
}

std::vector<Column> Parser::ParseCreateTableColumns() {
  std::vector<Column> result;
  while (true) {
    Column column;
    Token token = lexer_.GetNextToken();

    if (token.type == TOKEN_SEMI) {
      break;
    }

    if (token.type != TOKEN_KEYWORD) {
      throw SQLError(SYNTAX_ERROR);
    }

    column.name = token.value;
    token = lexer_.GetNextToken();

    if (token.type != TOKEN_KEYWORD) {
      throw SQLError(SYNTAX_ERROR);
    }

    column.type = DataTypeFromString(token.value);
    while (true) {
      token = lexer_.GetNextToken();
      if (token.type == TOKEN_COMMA || token.type == TOKEN_RBRACE) break;

      if (token.type != TOKEN_KEYWORD) {
        throw SQLError(SYNTAX_ERROR);
      }

      if (token.value == "PRIMARY") {
        token = lexer_.GetNextToken();
        if (token.value == "KEY") {
          column.attributes.push_back(PRIMARY_KEY);
        } else {
          throw SQLError(SYNTAX_ERROR);
        }
      } else if (token.value == "FOREIGN") {
        token = lexer_.GetNextToken();
        if (token.value == "KEY") {
          column.attributes.push_back(FOREIGN_KEY);
        } else {
          throw SQLError(SYNTAX_ERROR);
        }
      } else if (token.value == "NOT") {
        token = lexer_.GetNextToken();
        if (token.value == "NULL") {
          column.attributes.push_back(NOT_NULL);
        } else {
          throw SQLError(SYNTAX_ERROR);
        }
      }
    }
    result.push_back(column);
  }
  return result;
}

std::string Parser::ParseDropTable() {
  Token token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD) {
    throw SQLError(SYNTAX_ERROR);
  }

  ExpectSemicolon();
  return token.value;
}

void Parser::ExpectSemicolon() {
  Token token = lexer_.GetNextToken();
  if (token.type != TOKEN_SEMI) throw SQLError(SEMI_MISSED);
}

SelectFromModel Parser::ParseSelectFrom() {
  SelectFromModel result;
  while (true) {
    Token token = lexer_.GetNextToken();
    if (token.value == ALL) {
      result.select_all_columns = true;
      token = lexer_.GetNextToken();
      if (token.value != FROM) throw SQLError(SYNTAX_ERROR);
      break;
    }

    result.columns.push_back(token.value);
    token = lexer_.GetNextToken();

    if (token.value == FROM) break;
    if (token.type != TOKEN_COMMA) throw SQLError(SYNTAX_ERROR);
  }

  Token token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);

  result.table_name = token.value;

  token = lexer_.GetNextToken();
  if (token.type == TOKEN_SEMI) return result;
  if (token.value != WHERE) throw SQLError(SEMI_MISSED);

  ParseWhereCondition(result);

  return result;
}

InsertIntoModel Parser::ParseInsertInto(std::vector<Table>& tables) {
  InsertIntoModel result;

  Token token = lexer_.GetNextToken();

  bool parse_columns = false;
  std::vector<std::string> columns_name;
  std::pair<std::vector<Column>, std::vector<size_t>> columns_with_index;

  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);
  result.table_name = token.value;

  token = lexer_.GetNextToken();
  if (token.type == TOKEN_LBRACE) {
    columns_name = ParseColumnsInsert();
    parse_columns = true;
    token = lexer_.GetNextToken();
  }

  if (token.value != VALUES) throw SQLError(SYNTAX_ERROR);


  Table& table = FindTableByName(tables, result.table_name);
  if (parse_columns) {
    result.columns = columns_with_index.first;
  } else {
    result.columns = table.columns;
    std::for_each(table.columns.begin(), table.columns.end(), [&columns_name](Column& column) {
      columns_name.push_back(column.name);
    });
  }
  columns_with_index = FindColumnsByName(table.columns, columns_name);
  result.rows = ParseRows(columns_with_index, table);
  return result;
}

std::vector<Row> Parser::ParseRows(std::pair<std::vector<Column>, std::vector<size_t>>& columns, Table& table) {
  std::vector<Row> rows;

  while (true) {
    Token token = lexer_.GetNextToken();
    if (token.type != TOKEN_LBRACE) throw SQLError(SYNTAX_ERROR);
    rows.push_back(ParseRow(columns, table));

    token = lexer_.GetNextToken();

    if (token.type == TOKEN_SEMI) return rows;
    if (token.type != TOKEN_COMMA) throw SQLError(SYNTAX_ERROR);;
  }
}

Row Parser::ParseRow(std::pair<std::vector<Column>, std::vector<size_t>>& columns, Table& table) {
  int added_values = 0;
  Row row;
  for (int i = 0; i < table.columns.size(); ++i) {
    Token token = lexer_.GetNextToken();
    while (table.columns[i].name != columns.first[added_values].name && i < table.columns.size()) {
      if (VectorContains(table.columns[i].attributes, NOT_NULL)) {
        throw SQLError(COLUMN_IS_NOT_NULL);
      }
      row.fields.emplace_back(Null());
      ++i;
    }

      row.fields.push_back(GetValueOfType(table.columns[i].type, token.value));
      added_values++;

    token = lexer_.GetNextToken();
    if (token.type == TOKEN_RBRACE) {
      ++i;
      while (i < table.columns.size()) {
        if (VectorContains(table.columns[i].attributes, NOT_NULL)) {
          throw SQLError(COLUMN_IS_NOT_NULL);
        }
        row.fields.emplace_back(Null());
        ++i;
      }
      break;
    }
}
  if (added_values != columns.first.size()) throw SQLError(SYNTAX_ERROR);

  return row;
}

void Parser::ParseWhereCondition(ModelWithConditions& model_with_conditions) {
  std::stack<std::vector<Operand>> stack_operand;
  std::stack<ConditionTypes> conditions;

  while (true) {
    Token token = lexer_.GetNextToken();
    if (token.type == TOKEN_SEMI || token.type == TOKEN_END) break;
    else if (token.type == TOKEN_LBRACE) {
      conditions.push(CONDITION_BRACE);
    } else if (token.type == TOKEN_RBRACE) {
      while (conditions.top() != CONDITION_BRACE) {
        MergeOperandsBasedOnCondition(stack_operand, conditions.top());
        conditions.pop();
      }
      conditions.pop();
    } else if (token.value == "AND") {
      while (!conditions.empty() && conditions.top() == CONDITION_OR) {
        MergeOperandsBasedOnCondition(stack_operand, conditions.top());
        conditions.pop();
      }
      conditions.push(CONDITION_AND);
    } else if (token.value == "OR") {
      conditions.push(CONDITION_OR);
    } else if (token.type == TOKEN_KEYWORD) {
      Operand operand = ParseOperand(token.value);
      stack_operand.push({operand});
    } else if (token.type != TOKEN_KEYWORD) throw SQLError(SEMI_MISSED);
  }

  while (!conditions.empty()) {
    MergeOperandsBasedOnCondition(stack_operand, conditions.top());
    conditions.pop();
  }
  while (!stack_operand.empty()) {
    model_with_conditions.conditions.push_back(stack_operand.top());
    stack_operand.pop();
  }
}
Operand Parser::ParseOperand(std::string& field_name) {
  Operand operand;
  operand.field = field_name;

  Token token = lexer_.GetNextToken();

  //TODO: Parse from token.type, not string
  ComparisonOperator comp_operator = GetComparisonOperatorFromString(token.value);
  operand.comparison_operator = comp_operator;

  token = lexer_.GetNextToken();
  if (comp_operator == COMPARISON_IS_NOT_NULL || comp_operator == COMPARISON_IS_NULL) {
    operand.value = "";
  }
  else if (token.type != TOKEN_KEYWORD && token.type != TOKEN_STRING) throw SQLError(SYNTAX_ERROR);
  else { operand.value = token.value; }
  return operand;
}
void Parser::MergeOperandsBasedOnCondition(std::stack<std::vector<Operand>>& stack_operand, ConditionTypes& condition) {
  if (condition == CONDITION_OR) return;
  if (condition == CONDITION_AND) {
    std::vector<Operand> second_operand = stack_operand.top();
    stack_operand.pop();
    std::vector<Operand>& first_operand = stack_operand.top();

    first_operand.insert(
        first_operand.end(),
        std::make_move_iterator(second_operand.begin()),
        std::make_move_iterator(second_operand.end())
    );
  }
}
std::vector<std::string> Parser::ParseColumnsInsert() {
  std::vector<std::string> columns_name;
  while (true) {
    Token token = lexer_.GetNextToken();
    if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);
    columns_name.push_back(token.value);

    token = lexer_.GetNextToken();
    if (token.type == TOKEN_RBRACE) break;
    else if (token.type != TOKEN_COMMA) throw SQLError(SYNTAX_ERROR);
  }

  return columns_name;
}
DeleteFromModel Parser::ParseDelete(std::vector<Table>& tables) {
  DeleteFromModel result;
  Token token = lexer_.GetNextToken();
  if (token.value != FROM) throw SQLError(SYNTAX_ERROR);

  token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);

  result.table_name = token.value;
  Table& table = FindTableByName(tables, result.table_name);

  token = lexer_.GetNextToken();
  if (token.type == TOKEN_SEMI) {
    result.delete_all = true;
    return result;
  } else if (token.value != WHERE) throw SQLError(SYNTAX_ERROR);

  ParseWhereCondition(result);
  return result;
}
