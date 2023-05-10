//
// Created by Даник 💪 on 08.05.2023.
//
#include "structures/Column.h"
#include "structures/Table.h"

#include <vector>
#include <map>

static std::pair<std::vector<Column>, std::vector<size_t>> FindColumnsByName(const std::vector<Column>& columns,
                                                const std::vector<std::string>& columns_to_select) {
  std::pair<std::vector<Column>, std::vector<size_t>> result;
  result.first.reserve(columns_to_select.size());
  result.second.reserve(columns_to_select.size());

  for (auto& el : columns_to_select) {
    auto itr = std::find_if(columns.cbegin(), columns.cend(), [&el](const Column& column) {
      return column.name == el;
    });
    if (std::end(columns) == itr) throw SQLError(COLUMN_NOT_FOUND);

    result.first.emplace_back(*itr);
    result.second.emplace_back(std::distance(columns.begin(), itr));
  }

  return result;
}

// Returns map where key is name and value is a pair of column with this name and it's index.
static std::map<std::string, std::pair<Column, size_t>> GetMapOfColumnsIndexByName(const std::vector<Column>& columns) {
  std::map<std::string, std::pair<Column, size_t>> result;

  for (size_t i = 0; i < columns.size(); ++i) {
    result.insert({columns[i].name, std::make_pair(columns[i], i)});
  }

  return result;
}

static possible_data_types GetValueOfType(DataTypes type, std::string& value) {
  std::string lower_value = value;
  switch (type) {
    case Int:
      try {
        return std::stoi(value);
      } catch (std::exception& e) {
        throw SQLError(MISMATCHED_DATA_TYPE);
      }
    case Bool:
      std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      if (lower_value == "true") return true;
      else if (lower_value == "false") return false;
      else {
        throw SQLError(MISMATCHED_DATA_TYPE);
      }
    case Float:
      try {
        return std::stof(value);
      } catch (std::exception& e) {
        throw SQLError(MISMATCHED_DATA_TYPE);
      }
    case Double:
      try {
        return std::stod(value);
      } catch (std::exception& e) {
        throw SQLError(MISMATCHED_DATA_TYPE);
      }
    case Varchar:return value;
  }
}

static Table& FindTableByName(std::vector<Table>& tables, std::string& name) {
  auto table_iter = std::find_if(tables.begin(), tables.end(), [&name](Table& table) {
    return table.name == name;
  });
  if (table_iter == std::end(tables)) {
    throw SQLError(TABLE_NOT_FOUND);
  }

  return *table_iter;

}