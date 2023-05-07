//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_TABLE_H_
#define LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_TABLE_H_

#include <string>
#include "Columns.h"

struct Table {
  std::string name;
  std::vector<Column> columns;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_TABLE_H_
