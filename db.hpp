/*
Notes:

DbTable:
1. Manages a single table (like a dataframe).
2. Allows dynamic adjustment of rows and columns.

Database:
1. Manages a collection of multiple tables (like multiple dataframes).
2. Provides functionality for creating, deleting, and accessing tables by name.

By using both DbTable and Database, you can dynamically manage complex datasets with multiple tables, each with its own rows and columns! 
*/

#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <map>
#include <string>

#include "db_table.hpp"

class Database {
public:
    void CreateTable(const std::string& table_name);
    void DropTable(const std::string& table_name);
    DbTable& GetTable(const std::string& table_name);

    Database() = default;
    Database(const Database& rhs);
    Database& operator=(const Database& rhs);
    ~Database();
    friend std::ostream& operator<<(std::ostream& os, const Database& db);

private:
  std::map<std::string, DbTable*> tables_;  // maps table name -> table. table_ is an instance of db_table.
  //In simple words, it gives a table that can dynamically adjusting its row/col a name and saved them into a map.
};

std::ostream& operator<<(std::ostream& os, const Database& db);

#endif