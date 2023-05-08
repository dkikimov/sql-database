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
  InsertIntoModel ParseInsertInto();

 private:
  void ExpectSemicolon();
  std::vector<Column> ParseColumns();
  Lexer& lexer_;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
