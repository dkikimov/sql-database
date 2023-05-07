//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_
#define LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_

#include <vector>
#include "QueryResult.h"
#include "../lexer/Lexer.h"
#include "structures/Table.h"

class MyCoolDB {
 public:
  MyCoolDB() = default;

  std::vector<QueryResult> ExecuteCommand(const char* request);
 private:
  std::vector<Table> tables_;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_
