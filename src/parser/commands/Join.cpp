//
// Created by Даник 💪 on 20.05.2023.
//
#include "../Parser.h"
#include "../../db/algorithms.cpp"
#include "../../structures/Constants.h"

std::pair<JoinModel, Token> Parser::ParseJoin(SelectFromModel& select_from_model) {
  JoinModel result;
  Token token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);

  result.table_to_join = token.value;
  select_from_model.join = true;

  for (const std::string& column : select_from_model.columns) {
    result.all_columns.push_back(column);
    auto splitted_column = SplitStringByDelimiter(column, '.');
    if (splitted_column.size() != 2) throw SQLError(SYNTAX_ERROR);
    if (splitted_column[0] == select_from_model.table_name) {
      result.columns_1.push_back(splitted_column[1]);
    } else if (splitted_column[0] == result.table_to_join) {
      result.columns_2.push_back(splitted_column[1]);
    } else throw SQLError(TOO_MUCH_ARGUMENTS);
  }

  token = lexer_.GetNextToken();
  if (token.value != ON) throw SQLError(SYNTAX_ERROR);

  return std::make_pair(result, ParseCondition(result));
}