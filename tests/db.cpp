//
// Created by Даник 💪 on 07.05.2023.
//

#include "gtest/gtest.h"
#include "src/db/MyCoolDB.h"

MyCoolDB CreateDefaultTable() {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE table_name (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  return db;
}

TEST(DB, CreateTable) {
  MyCoolDB db;

  auto e = db.ExecuteCommand("CREATE TABLE table_name (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  ASSERT_EQ(e.size(), 1);

  Column id_column = Column("id", std::vector<Attributes>{PRIMARY_KEY}, Int);
  Column name_column = Column("name", std::vector<Attributes>{NOT_NULL}, Varchar);

  std::vector<Column> columns{
      id_column,
      name_column
  };

  ASSERT_EQ(e[0].columns, columns);
  ASSERT_EQ(db.GetTables()[0].columns, columns);
}

TEST(DB, DropTable) {
  MyCoolDB db = CreateDefaultTable();

  db.ExecuteCommand("DROP TABLE table_name;");
  ASSERT_EQ(db.GetTables().size(), 0);
}
