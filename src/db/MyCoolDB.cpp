//
// Created by Даник 💪 on 06.05.2023.
//

#include "MyCoolDB.h"
#include "../structures//Constants.h"
#include "structures/Table.h"
#include "../parser/Parser.h"

#include <sstream>

std::vector<QueryResult> MyCoolDB::ExecuteCommand(const char* request) {
  Lexer lexer(request);
  std::vector<QueryResult> result;
  Parser parser;

  while (true) {
      Token token = lexer.GetNextToken();
      if (token.type == TOKEN_END) {
        break;
      }

      if (token.value == "CREATE") {
        token = lexer.GetNextToken();
        if (token.value == "TABLE") {
          Table table = parser.ParseCreateTable(lexer);
          tables_.push_back(table);
          result.emplace_back(true, std::vector<Row>(), table.columns);
        }
      } else {
        throw SQLError(SYNTAX_ERROR);
      }
  }
  return result;
}
