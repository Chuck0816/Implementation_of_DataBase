# Implementation of a database

*This repository serves as an introduction to the implementation of a basic database, developed as part of the Machine Project titled "Implementation of a naive Database" for CS128 at the University of Illinois at Urbana-Champaign.*

### void*

Here, ```array[0]``` is a void*, but you know it points to a std::string. 
By using s```tatic_cast<std::string*>```, you tell the compiler to treat the void* as a pointer to a std::string.
Then, dereferencing this pointer (*) gives you the actual std::string object.

A ```void**``` is used instead of a void* because it can represent an array of ```void*``` pointers, with each pointer in the array pointing to dynamically allocated objects of various type, namely Name, UIN, GPA. 

```cpp
#include <iostream>
#include <string>

int main() {
  void** array = new void*[3];
  array[0] = static_cast<void*>(new std::string("Michael"));
  array[1] = static_cast<void*>(new int(7));
  array[2] = static_cast<void*>(new double(4.0));

  std::string& str = *(static_cast<std::string*>(
      array[0]));  // to access the object pointed to by array[0]
  int& i = *(static_cast<int*>(
      array[1]));  // to access the object pointed to by array[1]
  double& d = *(static_cast<double*>(
      array[2]));  // to access the object pointed to by array[2]

  std::cout << str << '\t' << i << '\t' << d << std::endl;

  delete static_cast<std::string*>(array[0]);
  delete static_cast<int*>(array[1]);
  delete static_cast<double*>(array[2]);
  delete[] array;
}
```
Output: Michael    7     4



### initializer_list

std::initializer_list allows you to pass a fixed list of values of the same type to a function or constructor using brace-enclosed syntax.
```cpp
#include <initializer_list>
#include <iostream>
#include <string>

void InitializerListPractice(std::initializer_list<std::string> init_list);

int main() { InitializerListPractice({"Howdy", "World", "1"}); }

void InitializerListPractice(std::initializer_list<std::string> init_list) {
  if (init_list.size() == 0) {
    std::cout << "init_list is empty" << std::endl;
  }
  for (const std::string& str : init_list) {
    std::cout << str << std::endl;
  }
}
```




## Table Description



### Columns
*We will store column descriptons and data types independently of the data records.*

Column descriptors and data types will be stored in the DbTable's ```std::vector<std::pair<srd::string, DataType>> col_descs_```

```col_desce_.at(0)``` points to a pair that contains "Name" and type of "Name" variable. 

```col_desce_.at(0).first``` -> "NAME"      ```col_desce_.at(0).second``` -> ```DataType::kString```

Possible DataType: ```DataType::kString ; DataType::kDouble ; DataType::kDouble```. Technically, std::string includes 'NAME', 'UIN', 'GPA', and DataType stores their types by definition.




### Rows
*We will store row descriptons and data types independently of the data records.* 

Specifically, table rows are stored in the private data member```std::map<unsigned int, void**> rows_```. We also add data member next_unique_id_ as a identifier when inserting a row to the table. After the insertion, the next_unique_id_ increment by one.

rows.at(0) assesses the data in index 0, while the voids** stores Name, UIN and GPA of a given row.(refer to the 1.1)

Ideally, ```row_col_capcacity_``` should be larger that ```col_descs_size()``` so that with oversize row arrays, we can add columns until ```col_descs_.size() == row_col_capacity```, ensuring no resize processes needed. However, if ```col_descs_.size()``` equals ```row_col_capacity```, we are responsible for resizing every row array in the table.

Lets say we have Mia's record by excuting```void** row = rows_at(0)```. To read the record's name field, we would use ```col_deses_.at(0).second```(refers to the Columns) returns ```DataType::kString```. To read the data, we could use ```*(static_cast<std:string*> (row[0]))```



### Steps of Retriving a name.
An example of void**: 
Name: John Doe
UIN: 123456
GPA: 3.8
```void** row = row_.at(0)``` : loops up the value mapped to in ```rows_``` by key 0. To access the data stored in Name column, we use ```void* row_col0 = row[0]```. Afterwards, we want to retrive the name using the methods to discussed in 1.1. Refer to the diagram "Retriving the Name"





## Database Description
The database type will have a single data member tables_ that will map a table name (```std::string```) to a DbTable pointer (DbTable*), which stores the address of a dynamically allocated table object. All interactions with the tables owned by the database will be through tables_. Refer to the diagram "Database description".



## Different functions of db_table files and db_hpp files

db_Table.hpp:
1. Manages a single table (like a dataframe).
2. Allows dynamic adjustment of rows and columns.

db.hpp:
1. Manages a collection of multiple tables (like multiple dataframes).
2. Provides functionality for creating, deleting, and accessing tables by name.

By using both DbTable and Database, we can dynamically manage complex datasets with multiple tables, each with its own rows and columns!






