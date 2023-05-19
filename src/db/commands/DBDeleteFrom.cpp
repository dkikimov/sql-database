//
// Created by Даник 💪 on 19.05.2023.
//

#include "../MyCoolDB.h"
#include "../algorithms.cpp"

void MyCoolDB::DeleteFrom(DeleteFromModel& delete_from_model) {
  Table& table = FindTableByName(tables_, delete_from_model.table_name);

  if (delete_from_model.delete_all) {
    return table.rows.clear();
  }

  std::vector<Row> rows;
  auto columns = GetColumnsMap(table.columns);
  SelectRowsByConditionTo(table.rows, columns, delete_from_model.conditions, rows);

  table.rows.erase(std::remove_if(table.rows.begin(), table.rows.end(), [&](const Row& row) {
    return std::find(rows.begin(), rows.end(), row) != rows.end();
  }), table.rows.end());
}