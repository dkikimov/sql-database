//
// Created by Даник 💪 on 13.05.2023.
//
#include "MyCoolDB.h"
#include "../db/algorithms.cpp"
#include <fstream>

void MyCoolDB::Save(const std::string& path) {
  std::ofstream file(path);
  if (!file.good()) throw DBError(FILE_TO_WRITE_CANT_OPEN);

  file << tables_.size() << "\n";
  for (const Table& table : tables_) {
    file << table.name + " " << table.columns.size() << " " << table.rows.size() << "\n";

    for (const Column& column : table.columns) {
      file << column.name << " " << column.attributes.size() << " " << column.type << "\n";
      for (const Attributes& attribute : column.attributes) {
        file << attribute << "\n";
      }
    }

    for (const Row& row : table.rows) {
      for (int i = 0; i < row.fields.size(); ++i) {
        std::visit([&file, &table, &i](const auto& elem) {
          if (table.columns[i].type == Varchar) {
            file << "'" << elem << "'" << ' ';
          } else {
            file << elem << ' ';
          }
        }, row.fields[i]);
      }
      file << '\n';
    }
  }

  file.close();
}

void MyCoolDB::Open(const std::string& path) {
  std::ifstream file(path);
  if (!file.good()) throw DBError(FILE_TO_READ_CANT_OPEN);

  int tables_size;
  file >> tables_size;
  tables_.reserve(tables_size);

  for (int i = 0; i < tables_size; ++i) {
    int columns_size, rows_size;
    Table table;
    file >> table.name >> columns_size >> rows_size;
    table.columns.reserve(columns_size);
    table.rows.reserve(rows_size);

    for (int j = 0; j < columns_size; ++j) {
      int attributes_size, column_type;
      Column column;
      file >> column.name >> attributes_size >> column_type;
      column.attributes.reserve(attributes_size);
      column.type = static_cast<DataTypes>(column_type);
      for (int k = 0; k < attributes_size; ++k) {
        int attribute;
        file >> attribute;
        column.attributes.push_back(static_cast<Attributes>(attribute));
      }
      table.columns.push_back(column);
    }

    for (int j = 0; j < rows_size; ++j) {
      Row row;
      row.fields.reserve(columns_size);
      for (int k = 0; k < columns_size; ++k) {
        std::string value;
        char character = '\0';
        if (table.columns[k].type) {
          file >> std::ws;
          int apostrophe_count = 0;
          while (apostrophe_count != 2) {
            file.get(character);
            value += character;
            if (character == '\'') {
              ++apostrophe_count;
            }
          }
          value = value.substr(1, value.size() - 2);
        } else {
          file >> value;
        }
        if (value == "Null") {
          row.fields.emplace_back(Null());
        } else {
          row.fields.push_back(GetValueOfType(table.columns[k].type, value));
        }
      }
      table.rows.push_back(row);
    }
    tables_.push_back(table);
  }
}

