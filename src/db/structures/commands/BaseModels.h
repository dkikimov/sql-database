//
// Created by Даник 💪 on 13.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_BASEMODELS_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_BASEMODELS_H_

#include "./Condition.h"

class ModelWithConditions {
 public:
  std::vector<std::vector<Operand>> conditions;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_BASEMODELS_H_
