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

void InsertMultipleValuesIntoTable(MyCoolDB& db) {
  db.ExecuteCommand("INSERT INTO table_name VALUES (1, 'toy'), (2, 'phone');");
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

  std::vector<Row> expected_rows{
      Row({1, "toy"})
  };
  ASSERT_EQ(db.GetTables()[0].rows, expected_rows);
}

TEST(DB_ModifyingData, InsertIntoTableMultipleValues) {
  MyCoolDB db = CreateDefaultTable();
  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"}),
      Row({2, "phone"})
  };
  ASSERT_EQ(db.GetTables()[0].rows, expected_rows);
}

TEST(DB_ModifyingData, InsertOnlyOneColumn) {
  MyCoolDB db = CreateDefaultTable();

  db.ExecuteCommand("INSERT INTO table_name(name) VALUES ('toy');");

  std::vector<Row> expected_rows{
      Row({Null(), "toy"}),
  };
  ASSERT_EQ(db.GetTables()[0].rows, expected_rows);
}

TEST(DB_ModifyingData, InsertOnlyOneNotNullableColumn) {
  MyCoolDB db = CreateDefaultTable();

  ASSERT_THROW(db.ExecuteCommand("INSERT INTO table_name(id) VALUES (1);"), SQLError);
}

TEST(DB_SelectFrom, AllColumns) {
  MyCoolDB db = CreateDefaultTable();

  InsertIntoTable(db);
  std::vector<QueryResult> result = db.ExecuteCommand("SELECT * FROM table_name;");

  ASSERT_EQ(result.size(), 1);

  std::vector<possible_data_types> expected_result{
      1, "toy"
  };

  ASSERT_EQ(result[0].rows[0].fields, expected_result);
  ASSERT_EQ(result[0].columns, db.GetTables()[0].columns);

}

TEST(DB_SelectFrom, OneColumn) {
  MyCoolDB db = CreateDefaultTable();

  InsertIntoTable(db);
  std::vector<QueryResult> result = db.ExecuteCommand("SELECT id FROM table_name;");

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

TEST(DB_SelectFrom, WhereCondition_And) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"})
  };

  std::vector<QueryResult> result = db.ExecuteCommand("SELECT * FROM table_name WHERE id = 1 AND name = 'toy';");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_Or) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"}),
      Row({2, "phone"})
  };

  std::vector<QueryResult> result = db.ExecuteCommand("SELECT * FROM table_name WHERE id = 1 OR name = 'phone';");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_Or_And) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE (id = 1 OR name = 'phone') AND name = 'toy';");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_Or_And_2) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"}),
      Row({2, "phone"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE (id = 1 OR name = 'phone') AND name = 'toy' OR id = 2;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_BiggerThan) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"}),
      Row({2, "phone"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id > 0;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_BiggerMoreThan) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"}),
      Row({2, "phone"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id >= 1;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_BiggerMoreThanEmpty) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{};

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id >= 3;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_SmallerThan) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"}),
      Row({2, "phone"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id < 3;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_SmallerEqualsThan) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({1, "toy"}),
      Row({2, "phone"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id <= 2;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_SmallerEqualsEmpty) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{};

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id <= 0;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_SelectWhereIsNull) {
  MyCoolDB db = CreateDefaultTable();

  db.ExecuteCommand("INSERT INTO table_name(name) VALUES ('toy');");

  std::vector<Row> expected_rows{
      Row({Null(), "toy"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id IS NULL;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_SelectFrom, WhereCondition_SelectWhereIsNotNull) {
  MyCoolDB db = CreateDefaultTable();

  db.ExecuteCommand("INSERT INTO table_name(name) VALUES ('toy');");
  db.ExecuteCommand("INSERT INTO table_name VALUES (2, 'phone');");

  std::vector<Row> expected_rows{
      Row({2, "phone"})
  };

  std::vector<QueryResult>
      result = db.ExecuteCommand("SELECT * FROM table_name WHERE id IS NOT NULL;");

  ASSERT_EQ(result[0].rows, expected_rows);
}

TEST(DB_ModifyingData, DeleteAllRows) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  db.ExecuteCommand("DELETE FROM table_name;");
  ASSERT_EQ(db.GetTables()[0].rows.size(), 0);
}

TEST(DB_ModifyingData, DeleteRowsByCondition) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::vector<Row> expected_rows{
      Row({2, "phone"})
  };

  db.ExecuteCommand("DELETE FROM table_name WHERE id = 1;");
  ASSERT_EQ(db.GetTables()[0].rows, expected_rows);
}

TEST(DB_ModifyingData, UpdateRows) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  db.ExecuteCommand("UPDATE table_name SET name = 'car';");

  std::vector<Row> expected_rows {
      Row({1, "car"}),
      Row({2, "car"})
  };
  ASSERT_EQ(db.GetTables()[0].rows, expected_rows);
}

TEST(DB_ModifyingData, UpdateRowsWhereCondition) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  db.ExecuteCommand("UPDATE table_name SET name = 'car' WHERE id = 1;");

  std::vector<Row> expected_rows {
      Row({1, "car"}),
      Row({2, "phone"})
  };
  ASSERT_EQ(db.GetTables()[0].rows, expected_rows);
}

TEST(DB_ModifyingData, UpdateMultipleColumnsWhereCondition) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  db.ExecuteCommand("UPDATE table_name SET name = 'car', id = 3 WHERE id = 1;");

  std::vector<Row> expected_rows {
      Row({3, "car"}),
      Row({2, "phone"})
  };
  ASSERT_EQ(db.GetTables()[0].rows, expected_rows);
}

TEST(DB_File, SavingAndReadingFile) {
  MyCoolDB db = CreateDefaultTable();

  InsertMultipleValuesIntoTable(db);

  std::string path = "/Users/katsushooter/ITMO/programming/labwork-12-KatsuShooter/db.txt";
  db.Save(path);

  MyCoolDB db_2;
  db_2.Open(path);

  ASSERT_EQ(db, db_2);
}

TEST(DB_File, SavingAndReadingFile_2) {
  MyCoolDB db = CreateDefaultTable();

  db.ExecuteCommand("INSERT INTO table_name VALUES (1, 'hello world'), (2, 'goodbye world');");

  std::string path = "/Users/katsushooter/ITMO/programming/labwork-12-KatsuShooter/db.txt";
  db.Save(path);

  MyCoolDB db_2;
  db_2.Open(path);

  ASSERT_EQ(db, db_2);
}

TEST(DB_Joins, InnerJoin) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (1, 1, 2000), (2, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items INNER JOIN items_price ON items.id = items_price.item_id;");

  std::vector<Row> expected_rows{
      Row({"toy", 1, 2000}),
      Row({"phone", 2, 2500}),
  };

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
    Column({"items.name"}, {NOT_NULL}, Varchar),
    Column({"items_price.item_id"}, {}, Int),
    Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, InnerJoinMultipleConditions) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (3, 1, 2000), (4, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items INNER JOIN items_price ON items.id = items_price.item_id, items.id = items_price.id;");

  std::vector<Row> expected_rows{};

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, InnerJoinWhereCondition) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (1, 1, 2000), (2, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items INNER JOIN items_price ON items.id = items_price.item_id WHERE items.id = 1;");

  std::vector<Row> expected_rows{
      Row({"toy", 1, 2000}),
  };

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, LeftJoin) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (1, 1, 2000), (2, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items LEFT JOIN items_price ON items.id = items_price.item_id;");

  std::vector<Row> expected_rows{
      Row({1, "toy", 1, 2000}),
      Row({2, "phone", 2, 2500}),
  };

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.id"}, {PRIMARY_KEY}, Int),
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, LeftJoinMultipleConditions) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (3, 1, 2000), (4, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items LEFT JOIN items_price ON items.id = items_price.item_id, items.id = items_price.id;");

  std::vector<Row> expected_rows{};

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.id"}, {PRIMARY_KEY}, Int),
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, LeftJoinWhereCondition) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (1, 1, 2000), (2, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items LEFT JOIN items_price ON items.id = items_price.item_id WHERE items.id = 1;");

  std::vector<Row> expected_rows{
      Row({1, "toy", 1, 2000}),
  };

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.id"}, {PRIMARY_KEY}, Int),
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, RightJoin) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (1, 1, 2000), (2, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items RIGHT JOIN items_price ON items.id = items_price.item_id;");

  std::vector<Row> expected_rows{
      Row({"toy", 1, 1, 2000}),
      Row({"phone", 2, 2, 2500}),
  };

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.id"}, {PRIMARY_KEY}, Int),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, RightJoinMultipleConditions) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (3, 1, 2000), (2, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items RIGHT JOIN items_price ON items.id = items_price.item_id, items.id = items_price.id;");

  std::vector<Row> expected_rows{
      Row({"phone", 2, 2, 2500}),
  };

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.id"}, {PRIMARY_KEY}, Int),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}

TEST(DB_Joins, RightJoinWhereCondition) {
  MyCoolDB db;

  db.ExecuteCommand("CREATE TABLE items (id INT PRIMARY KEY, name VARCHAR NOT NULL);");
  db.ExecuteCommand("INSERT INTO items VALUES (1, 'toy'), (2, 'phone');");

  db.ExecuteCommand("CREATE TABLE items_price (id INT PRIMARY KEY, item_id INT, price INT);");
  db.ExecuteCommand("INSERT INTO items_price VALUES (1, 1, 2000), (2, 2, 2500);");

  auto res = db.ExecuteCommand("SELECT items.name, items_price.item_id, items_price.price FROM items RIGHT JOIN items_price ON items.id = items_price.item_id WHERE items.id = 1;");

  std::vector<Row> expected_rows{
      Row({"toy", 1, 1, 2000}),
  };

  ASSERT_EQ(res[0].rows, expected_rows);

  std::vector<Column> expected_columns{
      Column({"items.name"}, {NOT_NULL}, Varchar),
      Column({"items_price.id"}, {PRIMARY_KEY}, Int),
      Column({"items_price.item_id"}, {}, Int),
      Column({"items_price.price"}, {}, Int),
  };

  ASSERT_EQ(expected_columns, res[0].columns);
}