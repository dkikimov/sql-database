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
  Parser() = default;


  Table ParseCreateTable(Lexer& lexer);
 private:

  std::vector<Column> ParseColumns(Lexer& lexer);
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
