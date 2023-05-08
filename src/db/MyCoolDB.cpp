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

      }

      else if (token.value == INSERT) {
        token = lexer.GetNextToken();
        if (token.value == INTO) {
          const std::string table_name = parser.ParseDropTable();
          DropTable(table_name);
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
  auto table_iter = std::find_if(tables_.begin(), tables_.end(), [&select_from](Table& table) {
    return table.name == select_from.table_name;
  });
  if (table_iter == std::end(tables_)) {
    throw SQLError(TABLE_NOT_FOUND);
  }

  Table& table = *table_iter;
  if (select_from.select_all_columns) {
    return {true, table.rows, table.columns};
  }

  auto columns_id = FindIndexesOfElement(table.columns, select_from.columns);
  //TODO:
//  QueryResult result;
}
