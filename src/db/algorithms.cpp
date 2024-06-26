//
// Created by Даник 💪 on 08.05.2023.
//
#include "structures/Column.h"
#include "structures/Table.h"
#include "structures/commands/Condition.h"

#include <vector>
#include <map>
#include <sstream>

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
static std::map<std::string, std::pair<Column, size_t>> GetColumnsMap(const std::vector<Column>& columns) {
  std::map<std::string, std::pair<Column, size_t>> result;

  for (size_t i = 0; i < columns.size(); ++i) {
    result.insert({columns[i].name, std::make_pair(columns[i], i)});
  }

  return result;
}

static std::vector<Column> ConcatenateTableTwoColumnsWithPrefix(const Table& table_1, const Table& table_2) {
  std::vector<Column> result;

  for (auto column : table_1.columns) {
    column.name = table_1.name + "." + column.name;
    result.push_back(column);
  }

  for (auto column : table_2.columns) {
    column.name = table_2.name + "." + column.name;
    result.push_back(column);
  }

  return result;
}

static std::map<std::string, std::pair<Column, size_t>> GetColumnsMapPlusTableName(const Table& table) {
  std::map<std::string, std::pair<Column, size_t>> result;

  for (size_t i = 0; i < table.columns.size(); ++i) {
    result.insert({table.name + "." + table.columns[i].name, std::make_pair(table.columns[i], i)});
  }

  return result;
}

static std::map<std::string, std::pair<Column, size_t>> GetMapOfTablesColumns(const Table& table_1,
                                                                              const Table& table_2) {
  std::map<std::string, std::pair<Column, size_t>> result;

  for (size_t i = 0; i < table_1.columns.size(); ++i) {
    result.insert({table_1.name + "." + table_1.columns[i].name, std::make_pair(table_1.columns[i], i)});
  }

  for (size_t i = 0; i < table_2.columns.size(); ++i) {
    result.insert({table_2.name + "." + table_2.columns[i].name,
                   std::make_pair(table_2.columns[i], table_1.columns.size() + i)});
  }

  return result;
}

static std::vector<std::string> GetNamesOfColumnsWithPrefix(const std::vector<Column>& columns,
                                                            const std::string& prefix) {
  std::vector<std::string> result;

  result.reserve(columns.size());
  for (const Column& column : columns) {
    result.push_back(prefix + "." + column.name);
  }
  return result;
}

static possible_data_types GetValueOfType(DataTypes type, const std::string& value) {
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

template<typename T>
static bool VectorContains(const std::vector<T>& vec, const T& value) {
  auto itr = std::find(vec.begin(), vec.end(), value);
  if (std::end(vec) == itr) return false;
  return true;
}

template<typename T>
static std::vector<T> ConcatenateVectors(const std::vector<T>& vec_1, const std::vector<T>& vec_2) {
  std::vector<T> vec(vec_1);
  vec.insert(vec.end(), vec_2.begin(), vec_2.end());
  return vec;
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
static std::vector<std::string> SplitStringByDelimiter(const std::string& input, char delimiter) {
  std::vector<std::string> tokens;
  std::stringstream ss(input);
  std::string token;

  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

static Row ConcatenateRows(const Row& row_1, const Row& row_2, const Table& table_1, const Table& table_2) {
  Row row;

  for (int i = 0; i < table_1.columns.size(); ++i) {
    row.fields.push_back(row_1.fields[i]);
  }

  for (int i = 0; i < table_2.columns.size(); ++i) {
    row.fields.push_back(row_2.fields[i]);
  }

  return row;
}

static std::vector<std::string> AddPrefixToStrings(const std::vector<std::string>& strings, const std::string& prefix) {
  std::vector<std::string> res(strings);

  res.reserve(strings.size());
  for (auto& string : res) {
    string.insert(0, prefix + ".");
  }
  return res;
}