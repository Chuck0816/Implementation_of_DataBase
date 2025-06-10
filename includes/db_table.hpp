#ifndef TABLE_HPP
#define TABLE_HPP

#include <initializer_list>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

enum class DataType { kString, kDouble, kInt };

class DbTable {
public:
  DbTable() = default; // default constructor
  void AddColumn(const std::pair<std::string, DataType>& col_desc); /* this function is responsible for resizing the col.
  e.g. If the table only has two cols while we have three cols are needed for "Name, UIN, GPA"*/
  void DeleteColumnByIdx(unsigned int col_idx);
  void AddRow(const std::initializer_list<std::string>& col_data);
  void DeleteRowById(unsigned int id);

  DbTable(const DbTable& rhs);
  DbTable& operator=(const DbTable& rhs);
  ~DbTable();
  friend std::ostream& operator<<(std::ostream& os, const DbTable& table);
  /* The operator<< function is not a member of the DbTable class; it is a non-member function
  that interacts with std::ostream. To allow it to access private members of DbTable,
  we declare it as a friend of the DbTable class.*/
  std::vector<std::vector<std::string>> GetRows() const;
  const std::vector<std::pair<std::string, DataType>>& GetColumnDescriptions() const {
    return col_descs_;
}



  private:
  unsigned int next_unique_id_ = 0;
  unsigned int row_col_capacity_ = 2;
  std::map<unsigned int, void**> rows_;
  std::vector<std::pair<std::string, DataType>> col_descs_;
  void ResizeRows();// helper functions are included in the private class.
  void FreeRowMemory(unsigned int id);
};

#endif
