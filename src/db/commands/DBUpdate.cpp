//
// Created by Даник 💪 on 19.05.2023.
//
#include "../MyCoolDB.h"
#include "../algorithms.cpp"

void MyCoolDB::Update(UpdateModel& update_model) {
  Table& table = FindTableByName(tables_, update_model.table_name);


  auto columns_map = GetColumnsMap(table.columns);
  for (const auto& update_value : update_model.update_values) {
    if (auto column = columns_map.find(update_value.field); column == columns_map.end()) {
      throw SQLError(COLUMN_NOT_FOUND);
    }
  }

  if (update_model.update_all) {
    for (Row& row: table.rows) {
      // Updating all rows
      for (const auto& update_value : update_model.update_values) {
        row.fields[columns_map[update_value.field].second] = GetValueOfType(columns_map[update_value.field].first.type, update_value.value);
      }
    }

    return;
  }

  UpdateSelectedRows(table.rows, columns_map, update_model.conditions, update_model);
}

void MyCoolDB::UpdateSelectedRows(std::vector<Row>& rows,
                                  std::map<std::string, std::pair<Column, size_t>>& columns,
                                  const std::vector<std::vector<Operand>>& conditions, const UpdateModel& update_model) {
  for (auto& row : rows) {
    bool ok = false;
    for (auto& operands : conditions) {
      if (ok) break;
      bool matches;
      for (auto& operand : operands) {
        possible_data_types row_value = row.fields[columns[operand.field].second];
        if (operand.comparison_operator == COMPARISON_IS_NULL) {
          matches = std::holds_alternative<Null>(row_value);
        } else if (operand.comparison_operator == COMPARISON_IS_NOT_NULL) {
          matches = !std::holds_alternative<Null>(row_value);
        } else {
          possible_data_types
              value_must_be = GetValueOfType(columns[operand.field].first.type, operand.value);
          matches = CompareValuesBasedOnOperator(row.fields[columns[operand.field].second],
                                                 value_must_be,
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
      for (const auto& update_value : update_model.update_values) {
        row.fields[columns[update_value.field].second] = GetValueOfType(columns[update_value.field].first.type, update_value.value);
      }
    }
  }
}