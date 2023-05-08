//
// Created by Даник 💪 on 06.05.2023.
//

#include "Parser.h"
#include "../structures/Constants.h"
Parser::Parser(Lexer& lexer): lexer_(lexer) {}

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
        }
        else {
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
      break;
    }

    result.columns.push_back(token.value);

    token = lexer_.GetNextToken();
    if (token.type != TOKEN_COMMA) break;
  }

  Token token = lexer_.GetNextToken();
  if (token.value != FROM) throw SQLError(SYNTAX_ERROR);

  token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);

  result.table_name = token.value;
  return result;
}
InsertIntoModel Parser::ParseInsertInto() {
  InsertIntoModel result;
  return InsertIntoModel();
}
