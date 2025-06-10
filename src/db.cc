#include "db.hpp"
#include <stdexcept>



/* maps table name -> table. table_ is an instance of db_table.
In simple words, it gives a table that can dynamically adjusting its row/col a name and saved them into a map.*/
void Database::CreateTable(const std::string& table_name) {
    if (tables_.find(table_name) != tables_.end()) {
        throw std::invalid_argument("Table already exists");
    }
    tables_[table_name] = new DbTable();
}

// Drop a table
void Database::DropTable(const std::string& table_name) {
    if (tables_.find(table_name) == tables_.end()) {
        throw std::out_of_range("Table does not exist");
    }
    delete tables_[table_name];
    tables_.erase(table_name);
}

DbTable& Database::GetTable(const std::string& table_name) {
    if (tables_.find(table_name) == tables_.end()) {
        throw std::out_of_range("Table does not exist");
    }
    return *tables_[table_name];
}

Database::~Database() {
    for (auto& [table_name, table] : tables_) {
        delete table;
    }
}

Database::Database(const Database& rhs) {
    for (const auto& [table_name, table] : rhs.tables_) {
        tables_[table_name] = new DbTable(*table);
    }
}



/*This is a copy assignment operator for the Database class.

1. Self-assignment Check: If the current object is being assigned to itself (e.g., db = db),
the function immediately returns the object (*this) to avoid unnecessary work.

2. Clean Up Existing Resources:
Deletes all dynamically allocated memory (delete table) in the current object's tables_ map and clears the map to prepare for the new data.

3. Deep Copy of rhs: Creates deep copies of the tables in the rhs object and stores them in the current object's tables_ map.

*/

Database& Database::operator=(const Database& rhs) {
    if (this == &rhs) return *this;
    for (auto& [table_name, table] : tables_) {
        delete table;
    }
    /*
        for (auto& pair : tables_) {
        delete pair.second; // Delete the table (value of the pair)
    }
    tables_.clear(); // Clear the map
    */
    tables_.clear();

    for (const auto& [table_name, table] : rhs.tables_) {
        tables_[table_name] = new DbTable(*table);
    }
  return *this;
}


std::ostream& operator<<(std::ostream& os, const Database& db) {
    for (const auto& [table_name, table] : db.tables_) {
        os << "Table: " << table_name << "\n" << *table << "\n";
    }
    return os;
}


/* std::ostream& operator<<(std::ostream& os, const Database& db) {
    for (const auto& pair : db.tables_) {
        os << "Table: " << pair.first << "\n" << *pair.second << "\n";
    }
    return os;
} */
