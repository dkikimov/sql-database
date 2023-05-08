//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_ROW_H_
#define LABWORK_12_KATSUSHOOTER_SRC_ROW_H_

#include <vector>

class Row {
 public:
  std::vector<union {
                int intData;
              }> data;
  Row() {

  }
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_ROW_H_
