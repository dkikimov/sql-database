//
// Created by Даник 💪 on 08.05.2023.
//
#include "structures/Column.h"
#include "structures/Table.h"
#include "structures/commands/Condition.h"

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

static DataTypes GetTypeOfValue(std::string& value) noexcept {
  try {
    std::stoi(value);
    return Int;
  } catch (std::exception& e) {}

  try {
    std::string lower_value = value;
    std::transform(lower_value.begin(), lower_value.end(), lower_value.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    if (lower_value == "true" || lower_value == "false") return Bool;
  } catch (std::exception& e) {}

  try {
    std::stof(value);
    return Float;
  } catch (std::exception& e) {}

  try {
    std::stod(value);
    return Double;
  } catch (std::exception& e) {}
  return Varchar;
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

template <typename T>
static bool VectorContains(const std::vector<T>& vec, const T& value) {
  auto itr = std::find(vec.begin(), vec.end(), value);
  if (std::end(vec) == itr) return false;
  return true;
}

template<typename T>
static bool CompareValuesBasedOnOperator(const T& value1, const T& value2, ComparisonOperator comp_operator) {
  switch (comp_operator) {
    case COMPARISON_EQUALS:return value1 == value2;
    case COMPARISON_BIGGER:return value1 > value2;
    case COMPARISON_BIGGER_EQUALS:return value1 >= value2;
    case COMPARISON_SMALLER:return value1 < value2;
    case COMPARISON_SMALLER_EQUALS:return value1 <= value2;
    case COMPARISON_NOT_EQUALS:return value1 != value2;
    case COMPARISON_IS_NULL: return std::is_same<T, Null>::value;
    case COMPARISON_IS_NOT_NULL: return !std::is_same<T, Null>::value;
  }
}