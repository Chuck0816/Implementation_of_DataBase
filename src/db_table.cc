#include "db_table.hpp"


/*! IN a database, one of an important function is dynamically adjust the number of cols and number of rows. 
This is what we are trying to achieve in AddColumn, DeleteColumnByIdx functions*/



// Helper function used in AddColumn to resize all rows' capacity
void DbTable::ResizeRows() {
    row_col_capacity_ *= 2;
    for (auto& pair : rows_) {
        unsigned int id = pair.first;
        void** row = pair.second;
        void** new_row = new void*[row_col_capacity_];
        for (size_t i = 0; i < col_descs_.size(); ++i) {
            new_row[i] = row[i];
        }
        delete[] row;
        rows_[id] = new_row;
    }
}


// add a new col. 
void DbTable::AddColumn(const std::pair<std::string, DataType>& col_desc) {
    if (col_descs_.size() == row_col_capacity_) {
        ResizeRows();
    }
    // Add the new column description to the vector
    col_descs_.push_back(col_desc); /*!mark difference in name*/
    // For each row, add a default value for the new column
    for (auto& pair : rows_) { //this means you are iterating over all the elements (key-value pairs) of the std::map named rows_.
        void** row = pair.second;
        // Add a default value for the new column based on the data type
        if (col_desc.second == DataType::kString) {
            row[col_descs_.size() - 1] = new std::string("");
        } else if (col_desc.second == DataType::kDouble) {
            row[col_descs_.size() - 1] = new double(0.0);
        } else if (col_desc.second == DataType::kInt) {
            row[col_descs_.size() - 1] = new int(0);
        }
    }
}

// removing a col if we found this col is unnecessary. The specific col we are removing is index col_idx
void DbTable::DeleteColumnByIdx(unsigned int col_idx) {
    if (col_idx >= col_descs_.size()) {
        throw std::out_of_range("Col index out of range");
    }
    // if the size of col is 3, then the max index will be 2.
    if (col_descs_.size() == 1 && !rows_.empty()) {
        throw std::runtime_error(
            "fail to delete the last column with rows present");
    }
    // we cannot remove the last col.
    for (auto& pair : rows_) {
        void** row = pair.second;
        if (col_descs_[col_idx].second == DataType::kString) {
            delete static_cast<std::string*>(row[col_idx]);
        } else if (col_descs_[col_idx].second == DataType::kDouble) {
            delete static_cast<double*>(row[col_idx]);
        } else if (col_descs_[col_idx].second == DataType::kInt) {
            delete static_cast<int*>(row[col_idx]);
        }
    for (size_t i = col_idx; i < col_descs_.size() - 1; ++i) {
        row[i] = row[i + 1]; // resize
    }
}
    col_descs_.erase(col_descs_.begin() + col_idx);
    //!!col_descs_.erase(col_descs_.begin() + col_idx) is removing the column description at a specific index (col_idx) from the vector col_descs_
}



// add a new row below of bottom row. 
void DbTable::AddRow(const std::initializer_list<std::string>& col_data) {
    if (col_data.size() != col_descs_.size()) {
        throw std::invalid_argument("Column data size mismatch");
    } // std::initializer_list<std::string>& col_data serves as a collecttions of info intended to be added inside of the databse. Thus, its number should matched the num of col. 

    void** new_row = new void*[row_col_capacity_]; // represents the new row that will be added.
    // Using row_col_capacity_ instead of col_data.size() ensures that the row aligns with the table's fixed structure and can handle all potential columns.
    size_t i = 0;

    for (const auto& data : col_data) {
        if (col_descs_[i].second == DataType::kString) {
        new_row[i] = new std::string(data);
    } else if (col_descs_[i].second == DataType::kDouble) {
        new_row[i] = new double(std::stod(data));
    } else if (col_descs_[i].second == DataType::kInt) {
        new_row[i] = new int(std::stoi(data));
    }
        ++i;
    }
    rows_[next_unique_id_++] = new_row;
}


// delete a existing row by id
void DbTable::DeleteRowById(unsigned int id) {
    if (rows_.find(id) == rows_.end()) { // this essentiallly means if we did not find id.
    /* Comparing the result of find() with my_map.end() is the classic way to check for the existence of a key in C++ maps.
    It is the same as rows.contains(id), but contains function was introduced only after C++20. */
        throw std::out_of_range("Row ID does not exist");
    }
    FreeRowMemory(id);
    rows_.erase(id);
}



/*Both are necessary to follow the Rule of Three in C++
(if a class manages resources like dynamic memory, you should implement the destructor, copy constructor, and copy assignment operator).*/

// deep copy constructor
DbTable::DbTable(const DbTable& rhs):
    next_unique_id_(rhs.next_unique_id_),
    row_col_capacity_(rhs.row_col_capacity_),
    col_descs_(rhs.col_descs_) {
    for (const auto& [id, row] : rhs.rows_) {
    void** new_row = new void*[row_col_capacity_];
    for (size_t i = 0; i < col_descs_.size(); ++i) {
        if (col_descs_[i].second == DataType::kString) {
            new_row[i] = new std::string(*(static_cast<std::string*>(row[i])));
        } else if (col_descs_[i].second == DataType::kDouble) {
            new_row[i] = new double(*(static_cast<double*>(row[i])));
        } else if (col_descs_[i].second == DataType::kInt) {
            new_row[i] = new int(*(static_cast<int*>(row[i])));
        }
    }
    rows_[id] = new_row;
    }
}

// copy assignment operator
DbTable& DbTable::operator=(const DbTable& rhs) {
    if (this == &rhs) return *this;
    for (auto& [id, row] : rows_) {
        FreeRowMemory(id);
    }
    rows_.clear();
    next_unique_id_ = rhs.next_unique_id_;
    row_col_capacity_ = rhs.row_col_capacity_;
    col_descs_ = rhs.col_descs_;
    for (const auto& [id, row] : rhs.rows_) {
        void** new_row = new void*[row_col_capacity_];
        for (size_t i = 0; i < col_descs_.size(); ++i) {
        if (col_descs_[i].second == DataType::kString) {
            new_row[i] = new std::string(*(static_cast<std::string*>(row[i])));
        } else if (col_descs_[i].second == DataType::kDouble) {
            new_row[i] = new double(*(static_cast<double*>(row[i])));
        } else if (col_descs_[i].second == DataType::kInt) {
            new_row[i] = new int(*(static_cast<int*>(row[i])));
        }
    }
    rows_[id] = new_row;
    }
    return *this;
}

// destructor
DbTable::~DbTable() {
    for (auto& [id, row] : rows_) {
        FreeRowMemory(id);
    }
}


// Helper to free memory of a single row in destructor
void DbTable::FreeRowMemory(unsigned int id) {
    if (rows_.find(id) != rows_.end()) { // "we find id!!!". The condition if (rows_.find(id) != rows_.end()) is logically equivalent to if (rows_.contains(id))[opposite, seems wield]
        void** row = rows_[id];
        for (size_t i = 0; i < col_descs_.size(); ++i) {
            if (col_descs_[i].second == DataType::kString) {
            delete static_cast<std::string*>(row[i]);
        } else if (col_descs_[i].second == DataType::kDouble) {
            delete static_cast<double*>(row[i]);
        } else if (col_descs_[i].second == DataType::kInt) {
            delete static_cast<int*>(row[i]);
        }
    }
    delete[] row;
    }
}


std::ostream& operator<<(std::ostream& os, const DbTable& table) {
    for (size_t i = 0; i < table.col_descs_.size(); ++i) {
        os << table.col_descs_[i].first << "(";
    if (table.col_descs_[i].second == DataType::kString) {
        os << "std::string";
    } else if (table.col_descs_[i].second == DataType::kDouble) {
        os << "double";
    } else if (table.col_descs_[i].second == DataType::kInt) {
        os << "int";
    }
        os << ")";
    if (i < table.col_descs_.size() - 1) {
        os << ", ";
    }
    }
    os << "\n";
    for (const auto& [id, row] : table.rows_) {
        for (size_t i = 0; i < table.col_descs_.size(); ++i) {
        if (table.col_descs_[i].second == DataType::kString) {
          os << *(static_cast<std::string*>(row[i]));
        } else if (table.col_descs_[i].second == DataType::kDouble) {
          os << *(static_cast<double*>(row[i]));
        } else if (table.col_descs_[i].second == DataType::kInt) {
          os << *(static_cast<int*>(row[i]));
        }
        if (i < table.col_descs_.size() - 1) {
            os << ", ";
        }
    }
        os << "\n";
    }
    return os;
}


/*This function, DbTable::GetRows(), extracts all the rows of the table as a std::vector<std::vector<std::string>>, with all data as string*/
std::vector<std::vector<std::string>> DbTable::GetRows() const {
    std::vector<std::vector<std::string>> rows_output;
    rows_output.reserve(rows_.size()); // If you want to reserve for efficiency
    // rows_ is your map: std::unordered_map<unsigned int, void**> rows_;
    // col_descs_ is your vector of (column name, DataType)
    for (const auto& [id, row] : rows_) {
        std::vector<std::string> row_data;
        row_data.reserve(col_descs_.size());
        for (size_t i = 0; i < col_descs_.size(); ++i) {
            // Convert each cell to a string based on its DataType
            if (col_descs_[i].second == DataType::kString) {
                row_data.push_back(*static_cast<std::string*>(row[i]));
            } else if (col_descs_[i].second == DataType::kDouble) {
                double value = *static_cast<double*>(row[i]);
                row_data.push_back(std::to_string(value));
            } else if (col_descs_[i].second == DataType::kInt) {
                int value = *static_cast<int*>(row[i]);
                row_data.push_back(std::to_string(value));
            }
        }
        rows_output.push_back(std::move(row_data));
    }
    return rows_output;
}
