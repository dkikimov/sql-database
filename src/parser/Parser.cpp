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
  result.columns = ParseColumns();
  return result;
}

std::vector<Column> Parser::ParseColumns() {
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
  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);
  result.table_name = token.value;

  Table& table = FindTableByName(tables, token.value);

  token = lexer_.GetNextToken();
  if (token.value != VALUES) throw SQLError(SYNTAX_ERROR);

  result.rows = ParseRows(table.columns);

  return result;
}

std::vector<Row> Parser::ParseRows(std::vector<Column>& columns) {
  std::vector<Row> rows;

  while (true) {
    Token token = lexer_.GetNextToken();
    if (token.type != TOKEN_LBRACE) throw SQLError(SYNTAX_ERROR);
    rows.push_back(ParseRow(columns));

    token = lexer_.GetNextToken();

    if (token.type == TOKEN_SEMI) return rows;
    if (token.type != TOKEN_COMMA) throw SQLError(SYNTAX_ERROR);;
  }
}

Row Parser::ParseRow(std::vector<Column>& columns) {
  int added_values = 0;
  Row row;
  while (true) {
    Token token = lexer_.GetNextToken();

    row.fields.push_back(GetValueOfType(columns[added_values].type, token.value));
    added_values++;

    token = lexer_.GetNextToken();
    if (token.type == TOKEN_RBRACE) break;
  }
  if (added_values != columns.size()) throw SQLError(SYNTAX_ERROR);

  return row;
}

void Parser::ParseWhereCondition(SelectFromModel& select_from_model) {
  std::stack<std::vector<Operand>> stack_operand;
  std::stack<ConditionTypes> conditions;

  while (true){
    Token token = lexer_.GetNextToken();
    if (token.type == TOKEN_SEMI) break;
    if (token.type == TOKEN_LBRACE) {
      conditions.push(CONDITION_BRACE);
    } else if (token.type == TOKEN_RBRACE) {
      while (conditions.top() != CONDITION_BRACE) {
        MergeOperandsBasedOnCondition(stack_operand, conditions.top());
        conditions.pop();
      }
      conditions.pop();
    }

    if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);
    Operand operand = ParseOperand(token.value);
    stack_operand.push({operand});

    token = lexer_.GetNextToken();
    if (token.type == TOKEN_SEMI) break;
    if (token.value == "AND") {
      while (!conditions.empty() && conditions.top() == CONDITION_OR) {
        MergeOperandsBasedOnCondition(stack_operand, conditions.top());
        conditions.pop();
      }
      conditions.push(CONDITION_AND);
    } else if (token.value == "OR") {
      conditions.push(CONDITION_OR);
    }
  }

  while (!conditions.empty()) {
    MergeOperandsBasedOnCondition(stack_operand, conditions.top());
    conditions.pop();
  }
  while (!stack_operand.empty()) {
    select_from_model.conditions.push_back(stack_operand.top());
    stack_operand.pop();
  }
}
Operand Parser::ParseOperand(std::string& field_name) {
  Operand operand;
  operand.field = field_name;

  Token token = lexer_.GetNextToken();
  ComparisonOperator comp_operator = GetComparisonOperatorFromString(token.value);
  if (comp_operator != COMPARISON_IS) {
    operand.comparison_operator = comp_operator;
  } else {
    token = lexer_.GetNextToken();
    try {
      comp_operator = GetComparisonOperatorFromString(token.value);
    } catch (std::exception& e) {
      comp_operator = GetComparisonOperatorFromString(token.value);
    }
    operand.comparison_operator = comp_operator;
  }

  token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD && token.type != TOKEN_STRING) throw SQLError(SYNTAX_ERROR);
  operand.value = token.value;
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
