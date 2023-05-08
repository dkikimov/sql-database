//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_
#define LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_

#include <vector>
#include "structures/QueryResult.h"
#include "../lexer/Lexer.h"
#include "structures/Table.h"
#include "structures/commands/SelectFromModel.h"

class MyCoolDB {
 public:
  MyCoolDB() = default;

  std::vector<QueryResult> ExecuteCommand(const char* request);
  const std::vector<Table>& GetTables() const;
 private:
  std::vector<Table> tables_;

  void DropTable(const std::string& table_name);
  QueryResult SelectFrom(SelectFromModel& select_from);
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_
