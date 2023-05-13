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
    }

    else {
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

  SelectRowsByConditionTo(table, select_from, rows);

  if (select_from.conditions.empty()) {
    rows = table.rows;
  }

  if (select_from.select_all_columns) {
    return {true, rows, table.columns};
  }

  auto columns = FindColumnsByName(table.columns, select_from.columns);

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
  SelectRowsByConditionTo(table, delete_from_model, rows);

  table.rows.erase(std::remove_if(table.rows.begin(), table.rows.end(), [&](const Row& row) {
    return std::find(rows.begin(), rows.end(), row) != rows.end();
  }), table.rows.end());
}

void MyCoolDB::SelectRowsByConditionTo(Table& table, ModelWithConditions& model_with_conditions, std::vector<Row>& rows_to_push) {
  auto columns_index_map = GetMapOfColumnsIndexByName(table.columns);
  for (auto& row : table.rows) {
    bool ok = false;
    for (auto& operands : model_with_conditions.conditions) {
      if (ok) break;
      for (auto& operand : operands) {
        bool matches = false;
        possible_data_types row_value = row.fields[columns_index_map[operand.field].second];
        if (operand.comparison_operator == COMPARISON_IS_NULL) {
          matches = std::holds_alternative<Null>(row_value);
        } else if (operand.comparison_operator == COMPARISON_IS_NOT_NULL) {
          matches = !std::holds_alternative<Null>(row_value);
        } else {
          possible_data_types value_must_be = GetValueOfType(columns_index_map[operand.field].first.type, operand.value);
          matches = CompareValuesBasedOnOperator(row.fields[columns_index_map[operand.field].second],
                                                 value_must_be,
                                                 operand.comparison_operator);
        }

        if (matches) {
          ok = true;
        } else {
          ok = false;
          break;
        }
      }
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
