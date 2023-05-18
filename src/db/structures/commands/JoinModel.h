//
// Created by Даник 💪 on 14.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_JOINMODEL_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_JOINMODEL_H_

#include "../Column.h"
#include "BaseModels.h"
#include "../JoinType.h"
#include <string>

struct JoinModel: public ModelWithConditions {
  JoinType join_type;
  std::string table_to_join;
  std::vector<std::string> columns_1;
  std::vector<std::string> columns_2;
  std::vector<std::string> all_columns;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_JOINMODEL_H_
