//
// Created by Даник 💪 on 06.05.2023.
//

#include "MyCoolDB.h"
#include "../structures/Constants.h"
#include "structures/Table.h"
#include "../parser/Parser.h"
#include "./algorithms.cpp"

#include <set>

std::vector<QueryResult> MyCoolDB::ExecuteCommand(const char* request) {
  Lexer lexer(request);
  std::vector<QueryResult> result;
  Parser parser(lexer);

  while (true) {
    Token token = lexer.GetNextToken();
    if (token.type == TOKEN_END) {
      break;
    }

    if (token.value == CREATE) {
      token = lexer.GetNextToken();
      if (token.value == TABLE) {
        Table table = parser.ParseCreateTable();
        tables_.push_back(table);
        result.emplace_back(true, std::vector<Row>(), table.columns);
      }
    } else if (token.value == DROP) {
      token = lexer.GetNextToken();
      if (token.value == TABLE) {
        const std::string table_name = parser.ParseDropTable();
        DropTable(table_name);
      }
    } else if (token.value == SELECT) {
      SelectFromModel data = parser.ParseSelectFrom();
      result.push_back(SelectFrom(data));
    } else if (token.value == INSERT) {
      token = lexer.GetNextToken();
      if (token.value == INTO) {
        InsertIntoModel data = parser.ParseInsertInto(tables_);
        InsertInto(data);
      } else {
        throw SQLError(SYNTAX_ERROR);
      }
    } else if (token.value == DELETE) {
      DeleteFromModel data = parser.ParseDelete(tables_);
      DeleteFrom(data);
    } else {
      throw SQLError(SYNTAX_ERROR);
    }
  }
  return result;
}
void MyCoolDB::DropTable(const std::string& table_name) {
  bool deleted = std::erase_if(tables_, [&table_name](Table& table) {
    return table.name == table_name;
  });

  if (!deleted) {
    throw SQLError(TABLE_NOT_FOUND);
  }
}

const std::vector<Table>& MyCoolDB::GetTables() const {
  return tables_;
}

QueryResult MyCoolDB::SelectFrom(SelectFromModel& select_from) {
  Table& table = FindTableByName(tables_, select_from.table_name);
  std::vector<Row> rows;
  std::pair<std::vector<Column>, std::vector<size_t>> columns;

  if (select_from.join) {
    Table& second_table = FindTableByName(tables_, select_from.join_model.table_to_join);
    switch (select_from.join_model.join_type) {
      case JOIN_LEFT:
        columns = FindColumnsByName(ConcatenateTableTwoColumnsWithPrefix(table, second_table), ConcatenateVectors(GetNamesOfColumnsWithPrefix(table.columns, table.name), AddPrefixToStrings(select_from.join_model.columns_2, second_table.name)));
        break;
      case JOIN_RIGHT:
        columns = FindColumnsByName(ConcatenateTableTwoColumnsWithPrefix(table, second_table), ConcatenateVectors(AddPrefixToStrings(select_from.join_model.columns_1, table.name), GetNamesOfColumnsWithPrefix(second_table.columns, second_table.name)));
        break;
      case JOIN_INNER:
        columns = FindColumnsByName(ConcatenateTableTwoColumnsWithPrefix(table, second_table), select_from.join_model.all_columns);
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

  }

  else {
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

void MyCoolDB::InsertInto(InsertIntoModel& insert_into_model) {
  Table& table = FindTableByName(tables_, insert_into_model.table_name);

  table.rows.insert(
      table.rows.end(),
      std::make_move_iterator(insert_into_model.rows.begin()),
      std::make_move_iterator(insert_into_model.rows.end())
  );
}

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

bool MyCoolDB::operator==(const MyCoolDB& rhs) const {
  return tables_ == rhs.tables_;
}
bool MyCoolDB::operator!=(const MyCoolDB& rhs) const {
  return !(rhs == *this);
}
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
