//
// Created by Даник 💪 on 20.05.2023.
//
#include "../Parser.h"
std::string Parser::ParseDropTable() {
  Token token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD) {
    throw SQLError(SYNTAX_ERROR);
  }
  std::string table_name = token.value;

  token = lexer_.GetNextToken();
  if (token.type != TOKEN_SEMI) throw SQLError(SEMI_EXPECTED);
  return table_name;
}