//
// Created by Даник 💪 on 06.05.2023.
//

#include "MyCoolDB.h"
#include "../structures/Constants.h"
#include "structures/commands/UpdateModel.h"
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
    } else if (token.value == UPDATE) {
      UpdateModel data = parser.ParseUpdate();
      Update(data);
    } else {
      throw SQLError(SYNTAX_ERROR);
    }
  }
  return result;
}

const std::vector<Table>& MyCoolDB::GetTables() const {
  return tables_;
}

bool MyCoolDB::operator==(const MyCoolDB& rhs) const {
  return tables_ == rhs.tables_;
}
bool MyCoolDB::operator!=(const MyCoolDB& rhs) const {
  return !(rhs == *this);
}