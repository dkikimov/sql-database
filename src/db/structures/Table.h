//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_TABLE_H_
#define LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_TABLE_H_

#include <string>
#include "Column.h"
#include "Row.h"

struct Table {
  bool operator==(const Table& rhs) const {
    return name == rhs.name &&
        columns == rhs.columns &&
        rows == rhs.rows;
  }
  bool operator!=(const Table& rhs) const {
    return !(rhs == *this);
  }

  std::string name;
  std::vector<Column> columns;
  std::vector<Row> rows;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_STRUCTURES_TABLE_H_
