//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_QUERYRESULT_H_
#define LABWORK_12_KATSUSHOOTER_SRC_QUERYRESULT_H_
#include "Row.h"
#include "structures/Columns.h"

#include <vector>

struct QueryResult {
  QueryResult(bool successful, const std::vector<Row>& rows, const std::vector<Column>& columns)
      : successful(successful), rows(rows), columns(columns) {}
  bool successful;
  std::vector<Row> rows;
  std::vector<Column> columns;
};

#endif //LABWORK_12_KATSUSHOOTER_SRC_QUERYRESULT_H_
