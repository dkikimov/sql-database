//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
#define LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_

#include <sstream>
#include "../db/structures/Table.h"
#include "../lexer/Lexer.h"

class Parser {
 public:
  Parser(Lexer& lexer);


  Table ParseCreateTable();
  std::string ParseDropTable();
 private:
  void ExpectSemicolon();
  std::vector<Column> ParseColumns();
  Lexer& lexer_;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
