//
// Created by Даник 💪 on 06.05.2023.
//

#include "Parser.h"
#include "../structures/Errors.h"
Table Parser::ParseCreateTable(Lexer& lexer) {
  Table result;

  Token token = lexer.GetNextToken();

  if (token.type != TOKEN_KEYWORD) {
    throw SQLError(SYNTAX_ERROR);
  }

  result.name = token.value;
  token = lexer.GetNextToken();

  if (token.type != TokenTypes::TOKEN_LBRACE) {
    throw SQLError(SYNTAX_ERROR);
  }
  result.columns = ParseColumns(lexer);

  return result;
}
std::vector<Column> Parser::ParseColumns(Lexer& lexer) {
  std::vector<Column> result;
  while (true) {
    Column column;
    Token token = lexer.GetNextToken();

    if (token.type == TOKEN_SEMI) {
      break;
    }
    if (token.type != TOKEN_KEYWORD) {
      throw SQLError(SYNTAX_ERROR);
    }

    column.name = token.value;
    token = lexer.GetNextToken();

    if (token.type != TOKEN_KEYWORD) {
      throw SQLError(SYNTAX_ERROR);
    }

    column.type = DataTypeFromString(token.value);
    while (true) {
      token = lexer.GetNextToken();
      if (token.type == TOKEN_COMMA || token.type == TOKEN_RBRACE) break;

      if (token.type != TOKEN_KEYWORD) {
        throw SQLError(SYNTAX_ERROR);
      }

      if (token.value == "PRIMARY") {
        token = lexer.GetNextToken();
        if (token.type != TOKEN_KEYWORD) {
          throw SQLError(SYNTAX_ERROR);
        }
        if (token.value == "KEY") {
          column.attributes.push_back(PRIMARY_KEY);
        }
      } else if (token.value == "FOREIGN") {
        token = lexer.GetNextToken();
        if (token.type != TOKEN_KEYWORD) {
          throw SQLError(SYNTAX_ERROR);
        }
        if (token.value == "KEY") {
          column.attributes.push_back(FOREIGN_KEY);
        }
      } else if (token.value == "NOT") {
        token = lexer.GetNextToken();
        if (token.type != TOKEN_KEYWORD) {
          throw SQLError(SYNTAX_ERROR);
        }
        if (token.value == "NULL") {
          column.attributes.push_back(NOT_NULL);
        }
      }
    }
    result.push_back(column);
  }
  return result;
}
