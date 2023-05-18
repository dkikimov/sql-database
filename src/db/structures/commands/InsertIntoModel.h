//
// Created by Даник 💪 on 08.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_INSERTINTOMODEL_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_INSERTINTOMODEL_H_
#include <vector>
#include <string>
#include "../Row.h"

struct InsertIntoModel {
  std::vector<Row> rows;
  std::string table_name;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_STRUCTURES_COMMANDS_INSERTINTOMODEL_H_
