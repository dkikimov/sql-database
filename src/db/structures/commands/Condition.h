//
// Created by Даник 💪 on 09.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_CONDITION_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_CONDITION_H_

#include <string>
#include <vector>

enum ComparisonOperator {
  COMPARISON_EQUALS,
  COMPARISON_BIGGER,
  COMPARISON_BIGGER_EQUALS,
  COMPARISON_SMALLER,
  COMPARISON_SMALLER_EQUALS,
  COMPARISON_NOT_EQUALS,
  COMPARISON_IS_NULL,
  COMPARISON_IS_NOT_NULL,
};

static ComparisonOperator GetComparisonOperatorFromString(std::string& string) {
  if (string == "=") {
    return COMPARISON_EQUALS;
  } else if (string == "!=") {
    return COMPARISON_NOT_EQUALS;
  } else if (string == ">") {
    return COMPARISON_BIGGER;
  } else if (string == ">=") {
    return COMPARISON_BIGGER_EQUALS;
  } else if (string == "<") {
    return COMPARISON_SMALLER;
  } else if (string == "<=") {
    return COMPARISON_SMALLER_EQUALS;
  } else if (string == "IS NULL") {
    return COMPARISON_IS_NULL;
  } else if (string == "IS NOT NULL") {
    return COMPARISON_IS_NOT_NULL;
  }

  else {
    throw SQLError(UNKNOWN_COMPARISON_OPERATOR);
  }
}

enum ConditionTypes {
  CONDITION_AND,
  CONDITION_OR,
  CONDITION_BRACE
};

static ConditionTypes GetConditionFromString(std::string& string) {
  if (string == "AND") {
    return CONDITION_AND;
  } else if (string == "OR") {
    return CONDITION_OR;
  }

  else {
    throw SQLError(UNKNOWN_CONDITION);
  }
}

struct Operand {
  std::string field;
  std::string value;
  ComparisonOperator comparison_operator;
};

struct Condition {
  ConditionTypes type;
  std::vector<Operand> operands;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_CONDITION_H_
