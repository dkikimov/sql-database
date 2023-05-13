//
// Created by Даник 💪 on 13.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_DELETEFROMMODEL_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_DELETEFROMMODEL_H_

#include <vector>
#include <string>
#include "./Condition.h"

struct DeleteFromModel: public ModelWithConditions {
  std::vector<std::vector<Operand>> conditions;
  std::string table_name;
  bool delete_all = false;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_DELETEFROMMODEL_H_
