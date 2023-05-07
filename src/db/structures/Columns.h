//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_COLUMNS_H_
#define LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_COLUMNS_H_

#include <string>
#include <utility>
#include <vector>
#include "../../structures/Attributes.h"
#include "../../structures/DataTypes.h"

class Column {
 public:

  Column(): type(Int) {};
  explicit Column(std::string name, const std::vector<Attributes>& attributes, DataTypes type)
      : name(std::move(name)), attributes(attributes), type(type) {

  };
  bool operator==(const Column& rhs) const {
    return name == rhs.name &&
        attributes == rhs.attributes &&
        type == rhs.type;
  }
  bool operator!=(const Column& rhs) const {
    return !(rhs == *this);
  }

  std::string name;
  std::vector<Attributes> attributes;
  DataTypes type;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_COLUMNS_H_
