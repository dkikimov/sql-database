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

void InsertIntoTable(MyCoolDB& db) {
  db.ExecuteCommand("INSERT INTO table_name VALUES (1, 'toy');");
}

TEST(DB_ManagingTables, CreateTable) {
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

TEST(DB_ManagingTables, DropTable) {
  MyCoolDB db = CreateDefaultTable();

  db.ExecuteCommand("DROP TABLE table_name;");
  ASSERT_EQ(db.GetTables().size(), 0);
}

TEST(DB_ModifyingData, InsertIntoTable) {
  MyCoolDB db = CreateDefaultTable();

  InsertIntoTable(db);
  ASSERT_EQ(db.GetTables()[0].rows.size(), 1);
}

TEST(DB_SelectFrom, All_Columns) {
  MyCoolDB db = CreateDefaultTable();

  InsertIntoTable(db);
  std::vector<QueryResult> result = db.ExecuteCommand("SELECT * FROM table_name");

  ASSERT_EQ(result.size(), 1);

  std::vector<possible_data_types> expected_result{
      1, "toy"
  };

  ASSERT_EQ(result[0].rows[0].fields, expected_result);
  ASSERT_EQ(result[0].columns, db.GetTables()[0].columns);

}

TEST(DB_SelectFrom, One_Column) {
  MyCoolDB db = CreateDefaultTable();

  InsertIntoTable(db);
  std::vector<QueryResult> result = db.ExecuteCommand("SELECT id FROM table_name");

  ASSERT_EQ(result.size(), 1);

  std::vector<possible_data_types> expected_result{
      1
  };

  std::vector<Column> expected_columns{
      Column("id", std::vector<Attributes>{PRIMARY_KEY}, Int)
  };

  ASSERT_EQ(result[0].rows[0].fields, expected_result);
  ASSERT_EQ(result[0].columns, expected_columns);
}

