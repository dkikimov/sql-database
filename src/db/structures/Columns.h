//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_COLUMNS_H_
#define LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_COLUMNS_H_

#include <string>
#include <vector>
#include "../../structures/Attributes.h"
#include "../../structures/DataTypes.h"

struct Column {
  std::string name;
  std::vector<Attributes> attributes;
  DataTypes type;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_COLUMNS_H_
