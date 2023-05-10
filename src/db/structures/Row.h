//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_ROW_H_
#define LABWORK_12_KATSUSHOOTER_SRC_ROW_H_

#include <vector>
#include <string>
#include "../../structures/DataTypes.h"

struct Row {
  explicit Row(const std::vector<possible_data_types>& fields) : fields(fields) {}
  Row() = default;

  bool operator==(const Row& rhs) const {
    return fields == rhs.fields;
  }
  bool operator!=(const Row& rhs) const {
    return !(rhs == *this);
  }

  std::vector<possible_data_types> fields;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_ROW_H_
