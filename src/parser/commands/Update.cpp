//
// Created by Даник 💪 on 20.05.2023.
//
#include "../Parser.h"
#include "../../structures/Constants.h"

UpdateModel Parser::ParseUpdate() {
  UpdateModel result;

  Token token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);
  result.table_name = token.value;

  token = lexer_.GetNextToken();
  if (token.value != SET) throw SQLError(SYNTAX_ERROR);

  while (true) {
    token = lexer_.GetNextToken();
    if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);
    result.update_values.push_back(ParseOperand(token.value));

    token = lexer_.GetNextToken();
    if (token.type != TOKEN_COMMA) break;
  }

  if (token.type == TOKEN_SEMI) {
    result.update_all = true;
    return result;
  }

  if (token.value != WHERE) throw SQLError(SYNTAX_ERROR);

  token = ParseCondition(result);
  if (token.type != TOKEN_SEMI) throw SQLError(SEMI_EXPECTED);

  return result;
}