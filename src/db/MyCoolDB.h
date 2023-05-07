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
  const std::vector<Table>& GetTables() const;
 private:
  std::vector<Table> tables_;

  void DropTable(Lexer& lexer);
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_
