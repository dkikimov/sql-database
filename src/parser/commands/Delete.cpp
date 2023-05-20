//
// Created by Даник 💪 on 20.05.2023.
//
#include "../Parser.h"
#include "../../structures/Constants.h"
#include "../../db/algorithms.cpp"

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

  if (ParseCondition(result).type != TOKEN_SEMI) throw SQLError(SEMI_EXPECTED);

  return result;
}