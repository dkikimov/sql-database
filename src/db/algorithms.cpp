//
// Created by Даник 💪 on 08.05.2023.
//
#include <vector>
#include "structures/Column.h"
static std::vector<size_t> FindIndexesOfElement(const std::vector<Column>& columns, const std::vector<std::string>& columns_to_select) {
  std::vector<size_t> result;
  result.reserve(columns_to_select.size());
  for (auto& el: columns_to_select) {
    auto itr = std::find_if(columns.cbegin(), columns.cend(), [&el](const Column& column){
      return column.name == el;
    });
    result.push_back(std::distance(columns.begin(), itr));
  }

  return result;
}