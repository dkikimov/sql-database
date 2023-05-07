//
// Created by Даник 💪 on 06.05.2023.
//

#include <iostream>
#include "../src/lexer/Lexer.h"
#include "../src/db/MyCoolDB.h"

int main() {
  auto req =
                                 "\t CREATE TABLE table_name (id INT PRIMARY KEY); "
                                 "SELECT * FROM table_name; "
                                 "INSERT INTO table_name VALUES (id1), (id2);"
                                 "SELECT * FROM table_name WHERE id = 1; ";

  MyCoolDB db;

  auto e = db.ExecuteCommand("CREATE TABLE table_name (id INT PRIMARY KEY, name VARCHAR NOT NULL);");

  return 0;
}