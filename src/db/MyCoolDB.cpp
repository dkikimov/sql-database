//
// Created by Даник 💪 on 06.05.2023.
//

#include "MyCoolDB.h"
#include "../structures//Constants.h"
#include "structures/Table.h"
#include "../parser/Parser.h"
#include "./algorithms.cpp"

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
      }

      else if (token.value == DROP) {
        token = lexer.GetNextToken();
        if (token.value == TABLE) {
          const std::string table_name = parser.ParseDropTable();
          DropTable(table_name);
        }
      }

      else if (token.value == SELECT) {
        SelectFromModel data = parser.ParseSelectFrom();
        result.push_back(SelectFrom(data));
      }

      else if (token.value == INSERT) {
        token = lexer.GetNextToken();
        if (token.value == INTO) {
          InsertIntoModel data = parser.ParseInsertInto(tables_);
          InsertInto(data);
        }
      }

      else {
        throw SQLError(SYNTAX_ERROR);
      }
  }
  return result;
}
void MyCoolDB::DropTable(const std::string& table_name) {
  bool deleted = std::erase_if(tables_, [&table_name](Table& table){
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
  if (select_from.select_all_columns) {
    return {true, table.rows, table.columns};
  }

  auto columns = FindColumnsByName(table.columns, select_from.columns);

  std::vector<Row> added_rows;
  added_rows.reserve(table.rows.size());

  for (Row& row: table.rows) {
    Row changed_row;
    for (auto column_id: columns.second) {
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

