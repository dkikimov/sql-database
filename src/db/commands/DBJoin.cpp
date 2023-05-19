//
// Created by Даник 💪 on 19.05.2023.
//

#include "../MyCoolDB.h"
#include "../algorithms.cpp"

void MyCoolDB::JoinTablesTo(Table& table_1, Table& table_2, SelectFromModel& select_from, std::vector<Row>& rows) {
  auto columns_index_map = GetColumnsMapPlusTableName(table_1);
  auto columns_to_join_index_map = GetColumnsMapPlusTableName(table_2);

  for (const Row& row : table_1.rows) {
    for (const Row& row_to_join : table_2.rows) {
      bool ok = false;
      for (const auto& operands : select_from.join_model.conditions) {
        if (ok) break;
        bool matches;
        for (auto& operand : operands) {
          possible_data_types row_value = row.fields[columns_index_map[operand.field].second];
          if (operand.comparison_operator == COMPARISON_IS_NULL) {
            matches = std::holds_alternative<Null>(row_value);
          } else if (operand.comparison_operator == COMPARISON_IS_NOT_NULL) {
            matches = !std::holds_alternative<Null>(row_value);
          } else {
            possible_data_types row_to_join_value = row_to_join.fields[columns_to_join_index_map[operand.field].second];
            matches = CompareValuesBasedOnOperator(row_value,
                                                   row_to_join_value,
                                                   operand.comparison_operator);
          }
          if (!matches) {
            ok = false;
            break;
          }
        }
        if (matches) ok = true;
      }
      if (ok) {
        rows.push_back(ConcatenateRows(row,
                                       row_to_join,
                                       table_1,
                                       table_2));
      }
    }
  }
}