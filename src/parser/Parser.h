//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
#define LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_

#include <sstream>
#include "../db/structures/Table.h"
#include "../lexer/Lexer.h"
#include "../db/structures/commands/SelectFromModel.h"
#include "../db/structures/commands/InsertIntoModel.h"

class Parser {
 public:
  explicit Parser(Lexer& lexer);

  Table ParseCreateTable();
  std::string ParseDropTable();
  SelectFromModel ParseSelectFrom();
  InsertIntoModel ParseInsertInto(std::vector<Table>& tables);

 private:
  void ExpectSemicolon();
  std::vector<Column> ParseColumns();
  std::vector<Row> ParseRows(std::vector<Column>& columns);
  Row ParseRow(std::vector<Column>& columns);
  void ParseWhereCondition(SelectFromModel& select_from_model);

  Lexer& lexer_;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
