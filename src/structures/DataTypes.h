//
// Created by Даник 💪 on 06.05.2023.
//

#ifndef LABWORK_12_KATSUSHOOTER_SRC_DATATYPES_H_
#define LABWORK_12_KATSUSHOOTER_SRC_DATATYPES_H_

#include "Errors.h"
#include <string>

enum DataTypes {
  Int,
  Bool,
  Float,
  Double,
  Varchar
};

static DataTypes DataTypeFromString(std::string& string) {
  std::transform(string.begin(), string.end(), string.begin(),
                 [](unsigned char c){ return std::tolower(c); });

  if (string == "int") {
    return Int;
  } else if (string == "bool") {
    return Bool;
  } else if (string == "float") {
    return Float;
  } else if (string == "double") {
    return Double;
  } else if (string == "varchar") {
    return Varchar;
  }

  throw SQLError(UNKNOWN_DATA_TYPE);
}

#endif //LABWORK_12_KATSUSHOOTER_SRC_DATATYPES_H_
