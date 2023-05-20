//
// Created by Даник 💪 on 20.05.2023.
//
#include "../Parser.h"
#include "../../structures/Constants.h"

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
  if (token.value == INNER || token.value == LEFT || token.value == RIGHT) {
    JoinType join_type;
    if (token.value == INNER) join_type = JOIN_INNER;
    else if (token.value == LEFT) join_type = JOIN_LEFT;
    else if (token.value == RIGHT) join_type = JOIN_RIGHT;

    token = lexer_.GetNextToken();
    if (token.value != JOIN) throw SQLError(SYNTAX_ERROR);
    auto join_model = ParseJoin(result);
    join_model.first.join_type = join_type;

    token = join_model.second;
    result.join_model = join_model.first;
  }
  if (token.type == TOKEN_SEMI) return result;

  if (token.value != WHERE) throw SQLError(SEMI_EXPECTED);

  if (ParseCondition(result).type != TOKEN_SEMI) throw SQLError(SEMI_EXPECTED);

  return result;
}