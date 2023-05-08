//
// Created by Даник 💪 on 06.05.2023.
//

#include "MyCoolDB.h"
#include "../structures//Constants.h"
#include "structures/Table.h"
#include "../parser/Parser.h"

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
