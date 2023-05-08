//
// Created by Даник 💪 on 08.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DB_COMMANDS_SELECTFROM_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DB_COMMANDS_SELECTFROM_H_

#include <vector>
#include <string>

struct SelectFromModel {
  bool select_all_columns = false;
  std::vector<std::string> columns;
  std::string table_name;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_DB_COMMANDS_SELECTFROM_H_
