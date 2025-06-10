// ─────────────────────────────────────────────────────────────────────────────
// tests/tests.cc
// Catch-2 unit / integration tests for mp-6 naive database
// ─────────────────────────────────────────────────────────────────────────────
#include "catch.hpp"

#include "db.hpp"
#include "db_table.hpp"

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>


// ─────────────────────────────────────────────────────────────────────────────
//  Original DATABASE tests (your code) – untouched
// ─────────────────────────────────────────────────────────────────────────────
TEST_CASE("Database can create a table and retrieve it") {
  Database db;

  REQUIRE_NOTHROW(db.CreateTable("employees"));
  REQUIRE_NOTHROW(db.GetTable("employees"));
  REQUIRE_THROWS_AS(db.CreateTable("employees"), std::invalid_argument);
}

TEST_CASE("Database can drop an existing table") {
  Database db;
  db.CreateTable("test_table");

  REQUIRE_NOTHROW(db.DropTable("test_table"));
  REQUIRE_THROWS_AS(db.DropTable("test_table"), std::out_of_range);
}

TEST_CASE("GetTable throws when table doesn't exist") {
  Database db;
  REQUIRE_THROWS_AS(db.GetTable("no_such_table"), std::out_of_range);
}

TEST_CASE("Database copy constructor creates distinct copies") {
  Database db1;
  db1.CreateTable("t1");
  db1.CreateTable("t2");

  Database db2(db1);                 // copy-ctor

  REQUIRE_NOTHROW(db2.GetTable("t1"));
  REQUIRE_NOTHROW(db2.GetTable("t2"));

  REQUIRE_NOTHROW(db2.DropTable("t2"));
  REQUIRE_THROWS_AS(db2.GetTable("t2"), std::out_of_range);
  REQUIRE_NOTHROW(db1.GetTable("t2"));  // original unaffected
}

TEST_CASE("Database assignment operator copies tables") {
  Database db1;
  db1.CreateTable("original");

  Database db2;
  db2.CreateTable("temp");

  db2 = db1;                         // copy-assign

  REQUIRE_NOTHROW(db2.GetTable("original"));
  REQUIRE_THROWS_AS(db2.GetTable("temp"), std::out_of_range);

  REQUIRE_NOTHROW(db2.DropTable("original"));
  REQUIRE_THROWS_AS(db2.GetTable("original"), std::out_of_range);
  REQUIRE_NOTHROW(db1.GetTable("original"));  // source intact
}

// ─────────────────────────────────────────────────────────────────────────────
//  NEW DbTable tests
// ─────────────────────────────────────────────────────────────────────────────
TEST_CASE("DbTable column management and ResizeRows") {
    DbTable tbl;
  
    // Add enough columns to force two resizes (initial capacity is 4).
    for (int i = 0; i < 12; ++i)
      tbl.AddColumn({"c" + std::to_string(i), DataType::kInt});
  
    REQUIRE(tbl.GetRows().empty());
  
    // Add one row with 12 integers (stringified).
    tbl.AddRow({"0","1","2","3","4","5","6","7","8","9","10","11"});
  
    auto rows = tbl.GetRows();
    REQUIRE(rows.size() == 1);
    REQUIRE(rows[0][0] == "0");
    REQUIRE(rows[0][11] == "11");
  }

TEST_CASE("AddRow type conversions and size mismatch") {
  DbTable t;
  t.AddColumn({"name", DataType::kString});
  t.AddColumn({"age",  DataType::kInt});
  t.AddColumn({"gpa",  DataType::kDouble});

  SECTION("happy path") {
    t.AddRow({"Alice", "20", "3.75"});
    auto r = t.GetRows();
    REQUIRE(r.size() == 1);
    REQUIRE(r[0][0] == "Alice");
    REQUIRE(r[0][1] == "20");
    REQUIRE(r[0][2].substr(0,4) == "3.75");  // to_string may append zeros
  }

  SECTION("size mismatch throws") {
    REQUIRE_THROWS_AS(t.AddRow({"too", "few"}), std::invalid_argument);
  }
}

TEST_CASE("DeleteColumnByIdx (regular, oob, last-col protection)") {
  DbTable t;
  t.AddColumn({"a", DataType::kString});
  t.AddColumn({"b", DataType::kInt});
  t.AddRow({"x", "7"});

  SECTION("out of range") {
    REQUIRE_THROWS_AS(t.DeleteColumnByIdx(5), std::out_of_range);
  }

  SECTION("delete first column shifts data") {
    t.DeleteColumnByIdx(0);          // remove "a"
    auto v = t.GetRows();
    REQUIRE(v[0].size() == 1);
    REQUIRE(v[0][0] == "7");
  }

  SECTION("cannot delete last remaining column while rows present") {
    t.DeleteColumnByIdx(0);          // leaves one column
    REQUIRE_THROWS_AS(t.DeleteColumnByIdx(0), std::runtime_error);
  }
}

TEST_CASE("DeleteRowById works and throws for unknown id") {
  DbTable t;
  t.AddColumn({"val", DataType::kInt});
  t.AddRow({"1"});
  t.AddRow({"2"});

  REQUIRE_NOTHROW(t.DeleteRowById(0));
  REQUIRE_THROWS_AS(t.DeleteRowById(0), std::out_of_range);
  REQUIRE(t.GetRows().size() == 1);
}

TEST_CASE("DbTable deep copy constructor and assignment") {
  DbTable src;
  src.AddColumn({"txt", DataType::kString});
  src.AddRow({"hello"});

  DbTable copy(src);                 // deep copy ctor
  DbTable assigned;
  assigned = src;                    // copy assignment

  src.DeleteRowById(0);              // mutate original

  REQUIRE(copy.GetRows().size() == 1);
  REQUIRE(assigned.GetRows().size() == 1);
  REQUIRE(src.GetRows().empty());
}

TEST_CASE("operator<< prints something") {
  DbTable t;
  t.AddColumn({"n", DataType::kInt});
  t.AddRow({"42"});

  std::stringstream ss;
  ss << t;
  REQUIRE(!ss.str().empty());

  Database db;
  db.CreateTable("tab");
  db.GetTable("tab") = t;

  std::stringstream ss2;
  ss2 << db;
  REQUIRE(!ss2.str().empty());
}

// ─────────────────────────────────────────────────────────────────────────────
//  Additional Database edge-cases
// ─────────────────────────────────────────────────────────────────────────────
TEST_CASE("Dropping a table frees its name for reuse") {
  Database db;
  db.CreateTable("tmp");
  db.DropTable("tmp");
  REQUIRE_NOTHROW(db.CreateTable("tmp"));   // name can be re-used
}

TEST_CASE("Creating many tables and deep copying database") {
  Database db;
  for (int i = 0; i < 20; ++i) {
    db.CreateTable("t" + std::to_string(i));
  }
  Database copy(db);
  for (int i = 0; i < 20; ++i) {
    REQUIRE_NOTHROW(copy.GetTable("t" + std::to_string(i)));
  }
}