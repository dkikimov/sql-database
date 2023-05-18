//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
#define LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_

#include "../db/structures/Table.h"
#include "../lexer/Lexer.h"
#include "../db/structures/commands/SelectFromModel.h"
#include "../db/structures/commands/InsertIntoModel.h"
#include "../db/structures/commands/DeleteFromModel.h"
#include "../db/structures/commands/JoinModel.h"

#include <stack>

class Parser {
 public:
  explicit Parser(Lexer& lexer);

  Table ParseCreateTable();
  std::string ParseDropTable();
  SelectFromModel ParseSelectFrom();
  InsertIntoModel ParseInsertInto(std::vector<Table>& tables);
  DeleteFromModel ParseDelete(std::vector<Table>& tables);

 private:
  void ExpectSemicolon();
  std::vector<Column> ParseCreateTableColumns();
  std::vector<Row> ParseRows(std::pair<std::vector<Column>, std::vector<size_t>>& columns, Table& table);
  std::vector<std::string> ParseColumnsInsert();
  Row ParseRow(std::pair<std::vector<Column>, std::vector<size_t>>& columns, Table& table);

  std::pair<JoinModel, Token> ParseJoin(SelectFromModel& select_from_model);

  Token ParseCondition(ModelWithConditions& model_with_conditions);
  static void MergeOperandsBasedOnCondition(std::stack<std::vector<Operand>>& stack_operand, ConditionTypes& condition);

  Operand ParseOperand(std::string& field_name);


  Lexer& lexer_;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_PARSER_H_
