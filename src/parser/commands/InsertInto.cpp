//
// Created by Даник 💪 on 20.05.2023.
//
#include "../Parser.h"
#include "../../structures/Constants.h"
#include "../../db/algorithms.cpp"

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
  } else {
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