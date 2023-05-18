//
// Created by Даник 💪 on 18.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_UPDATEMODEL_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_UPDATEMODEL_H_

#include <vector>
#include <string>
#include "BaseModels.h"

struct UpdateModel: public WhereCondition {
  bool update_all = false;
  std::string table_name;
  std::vector<Operand> update_values;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_UPDATEMODEL_H_
