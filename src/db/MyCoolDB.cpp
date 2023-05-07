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
  Parser parser;

  while (true) {
      Token token = lexer.GetNextToken();
      if (token.type == TOKEN_END) {
        break;
      }

      if (token.value == CREATE) {
        token = lexer.GetNextToken();
        if (token.value == TABLE) {
          Table table = parser.ParseCreateTable(lexer);
          tables_.push_back(table);
          result.emplace_back(true, std::vector<Row>(), table.columns);
        }
      }

      else if (token.value == DROP) {
        token = lexer.GetNextToken();
        if (token.value == TABLE) {
          DropTable(lexer);
        }
      }

      else {
        throw SQLError(SYNTAX_ERROR);
      }
  }
  return result;
}
void MyCoolDB::DropTable(Lexer& lexer) {
  Token token = lexer.GetNextToken();
  if (token.type != TOKEN_KEYWORD) throw SQLError(SYNTAX_ERROR);

  bool deleted = std::erase_if(tables_, [&token](Table& table){
    return table.name == token.value;
  });

  if (!deleted) {
    throw SQLError(TABLE_NOT_FOUND);
  }

  token = lexer.GetNextToken();
  if (token.type != TOKEN_SEMI) throw SQLError(SYNTAX_ERROR);
}

const std::vector<Table>& MyCoolDB::GetTables() const {
  return tables_;
}
