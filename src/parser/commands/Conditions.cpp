//
// Created by Даник 💪 on 20.05.2023.
//
#include "../Parser.h"
#include "../../structures/Constants.h"

Token Parser::ParseCondition(WhereCondition& model_with_conditions) {
  std::stack<std::vector<Operand>> stack_operand;
  std::stack<ConditionTypes> conditions;

  Token token = lexer_.GetNextToken();
  while (true) {
    if (token.type == TOKEN_SEMI || token.value == WHERE) break;
    else if (token.type == TOKEN_LBRACE) {
      conditions.push(CONDITION_BRACE);
    } else if (token.type == TOKEN_RBRACE) {
      while (conditions.top() != CONDITION_BRACE) {
        MergeOperandsBasedOnCondition(stack_operand, conditions.top());
        conditions.pop();
      }
      conditions.pop();
    } else if (token.value == AND || token.value == ",") {
      while (!conditions.empty() && conditions.top() == CONDITION_OR) {
        MergeOperandsBasedOnCondition(stack_operand, conditions.top());
        conditions.pop();
      }
      conditions.push(CONDITION_AND);
    } else if (token.value == OR) {
      conditions.push(CONDITION_OR);
    } else if (token.type == TOKEN_KEYWORD) {
      Operand operand = ParseOperand(token.value);
      stack_operand.push({operand});
    } else if (token.type != TOKEN_KEYWORD) throw SQLError(SEMI_EXPECTED);
    token = lexer_.GetNextToken();
  }

  while (!conditions.empty()) {
    MergeOperandsBasedOnCondition(stack_operand, conditions.top());
    conditions.pop();
  }
  while (!stack_operand.empty()) {
    model_with_conditions.conditions.push_back(stack_operand.top());
    stack_operand.pop();
  }

  return token;
}
Operand Parser::ParseOperand(std::string& field_name) {
  Operand operand;
  operand.field = field_name;

  Token token = lexer_.GetNextToken();

  //TODO: Parse from token.type, not string
  ComparisonOperator comp_operator = GetComparisonOperatorFromString(token.value);
  operand.comparison_operator = comp_operator;

  if (comp_operator == COMPARISON_IS_NOT_NULL || comp_operator == COMPARISON_IS_NULL) {
    operand.value = "";
    return operand;
  }

  token = lexer_.GetNextToken();
  if (token.type != TOKEN_KEYWORD && token.type != TOKEN_STRING) throw SQLError(SYNTAX_ERROR);
  else { operand.value = token.value; }
  return operand;
}

void Parser::MergeOperandsBasedOnCondition(std::stack<std::vector<Operand>>& stack_operand, ConditionTypes& condition) {
  if (condition == CONDITION_OR) return;
  if (condition == CONDITION_AND) {
    std::vector<Operand> second_operand = stack_operand.top();
    stack_operand.pop();
    std::vector<Operand>& first_operand = stack_operand.top();

    first_operand.insert(
        first_operand.end(),
        std::make_move_iterator(second_operand.begin()),
        std::make_move_iterator(second_operand.end())
    );
  }
}