//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_
#define LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_

#include "structures/QueryResult.h"
#include "../lexer/Lexer.h"
#include "structures/Table.h"
#include "structures/commands/SelectFromModel.h"
#include "structures/commands/InsertIntoModel.h"
#include "structures/commands/DeleteFromModel.h"

#include <vector>
#include <map>

class MyCoolDB {
 public:
  MyCoolDB() = default;

  std::vector<QueryResult> ExecuteCommand(const char* request);
  [[nodiscard]] const std::vector<Table>& GetTables() const;

  void Save(const std::string& path);
  void Open(const std::string& path);
  bool operator==(const MyCoolDB& rhs) const;
  bool operator!=(const MyCoolDB& rhs) const;
 private:
  std::vector<Table> tables_;

  void DropTable(const std::string& table_name);
  QueryResult SelectFrom(SelectFromModel& select_from);
  void InsertInto(InsertIntoModel& insert_into_model);
  void DeleteFrom(DeleteFromModel& delete_from_model);
  static void JoinTablesTo(Table& table_1, Table& table_2, SelectFromModel& select_from, std::vector<Row>& rows);

  static void SelectRowsByConditionTo(const std::vector<Row>& rows,
                               std::map<std::string, std::pair<Column, size_t>>& columns,
                               const std::vector<std::vector<Operand>>& conditions,
                               std::vector<Row>& rows_to_push);
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_MYCOOLDB_H_
