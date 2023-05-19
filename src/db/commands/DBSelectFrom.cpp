//
// Created by Даник 💪 on 19.05.2023.
//
#include "../MyCoolDB.h"
#include "../algorithms.cpp"

QueryResult MyCoolDB::SelectFrom(SelectFromModel& select_from) {
  Table& table = FindTableByName(tables_, select_from.table_name);
  std::vector<Row> rows;
  std::pair<std::vector<Column>, std::vector<size_t>> columns;

  if (select_from.join) {
    Table& second_table = FindTableByName(tables_, select_from.join_model.table_to_join);
    switch (select_from.join_model.join_type) {
      case JOIN_LEFT:columns = FindColumnsByName(ConcatenateTableTwoColumnsWithPrefix(table, second_table),
                                                 ConcatenateVectors(GetNamesOfColumnsWithPrefix(table.columns,
                                                                                                table.name),
                                                                    AddPrefixToStrings(select_from.join_model.columns_2,
                                                                                       second_table.name)));
        break;
      case JOIN_RIGHT:columns = FindColumnsByName(ConcatenateTableTwoColumnsWithPrefix(table, second_table),
                                                  ConcatenateVectors(AddPrefixToStrings(select_from.join_model.columns_1,
                                                                                        table.name),
                                                                     GetNamesOfColumnsWithPrefix(second_table.columns,
                                                                                                 second_table.name)));
        break;
      case JOIN_INNER:columns = FindColumnsByName(ConcatenateTableTwoColumnsWithPrefix(table, second_table),
                                                  select_from.join_model.all_columns);
        break;
    }

    std::vector<Row> selected_rows;
    JoinTablesTo(table, second_table, select_from, selected_rows);

    if (!select_from.conditions.empty()) {
      auto joined_columns = GetMapOfTablesColumns(table, second_table);
      SelectRowsByConditionTo(selected_rows, joined_columns, select_from.conditions, rows);
    } else {
      rows = std::move(selected_rows);
    }

  } else {
    columns = FindColumnsByName(table.columns, select_from.columns);

    auto columns_map = GetColumnsMap(table.columns);
    SelectRowsByConditionTo(table.rows, columns_map, select_from.conditions, rows);

    if (select_from.conditions.empty()) {
      rows = table.rows;
    }

    if (select_from.select_all_columns) {
      return {true, rows, table.columns};
    }
  }

  std::vector<Row> added_rows;
  added_rows.reserve(rows.size());
  for (const Row& row : rows) {
    Row changed_row;
    for (auto column_id : columns.second) {
      changed_row.fields.push_back(row.fields[column_id]);
    }
    added_rows.push_back(changed_row);
  }

  return {true, added_rows, columns.first};
}

void MyCoolDB::SelectRowsByConditionTo(const std::vector<Row>& rows,
                                       std::map<std::string, std::pair<Column, size_t>>& columns,
                                       const std::vector<std::vector<Operand>>& conditions,
                                       std::vector<Row>& rows_to_push) {
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
      rows_to_push.push_back(row);
    }
  }
}