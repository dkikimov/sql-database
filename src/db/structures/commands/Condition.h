//
// Created by Даник 💪 on 09.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_CONDITION_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_CONDITION_H_

#include <string>
#include <vector>

enum ConditionTypes {
  CONDITION_AND,
  CONDITION_OR,
  CONDITION_IS,
  CONDITION_NULL,
};

static ConditionTypes GetConditionFromString(std::string& string) {
  if (string == "AND") {
    return CONDITION_AND;
  } else if (string == "OR") {
    return CONDITION_OR;
  } else if (string == "IS") {
    return CONDITION_IS;
  } else if (string == "NULL") {
    return CONDITION_NULL;
  }
  else {
    throw SQLError(UNKNOWN_CONDITION);
  }
}

struct Operand {
  std::string field;
  std::string value;
};

class Condition {
 public:
  virtual void AddOperand(Operand& operand) {};
 private:

  std::vector<Operand> operands_;
};

class AndCondition {
 public:

 private:
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_CONDITION_H_
