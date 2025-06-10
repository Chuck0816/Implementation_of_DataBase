/*********************************************************************
 *  test.cc – unit-/integration-tests for db_table.[hc]pp & db.[hc]pp *
 *                                                                   *
 *  Build (example)                                                  *
 *     g++ -std=c++17 test.cc db_table.cc db.cc -o tests             *
 *                                                                   *
 *  Run                                                              *
 *     ./tests                                                       *
 *********************************************************************/

 #include <cassert>
 #include <iostream>
 #include <stdexcept>
 #include <string>
 #include <vector>
 #include "db_table.hpp"
 #include "db.hpp"
 
 /*------------------------------------------------------------*
  | Tiny helper macros                                          |
  *------------------------------------------------------------*/
 #define REQUIRE(cond)                                                         \
   do {                                                                        \
     if (!(cond)) {                                                            \
       std::cerr << "REQUIRE failed: " #cond << "  (" << __FILE__ << ':'       \
                 << __LINE__ << ")\n";                                         \
       std::terminate();                                                       \
     }                                                                         \
   } while (false)
 
 #define REQUIRE_THROWS(expr)                                                  \
   do {                                                                        \
     bool threw = false;                                                       \
     try {                                                                     \
       (void)(expr);                                                           \
     } catch (...) {                                                           \
       threw = true;                                                           \
     }                                                                         \
     REQUIRE(threw);                                                           \
   } while (false)
 
 /*------------------------------------------------------------*
  | Pretty printer for a 2-D vector (for debugging)            |
  *------------------------------------------------------------*/
 void Dump(const std::vector<std::vector<std::string>>& rows) {
   for (const auto& r : rows) {
     std::cout << "  [ ";
     for (size_t i = 0; i < r.size(); ++i) {
       std::cout << r[i] << (i + 1 == r.size() ? " ]\n" : ", ");
     }
   }
 }
 
 /*------------------------------------------------------------*
  |  Tests for a single DbTable                                 |
  *------------------------------------------------------------*/
 void TestDbTable() {
   std::cout << ">> TestDbTable\n";
 
   DbTable tbl;
 
   /*--- 1. add three columns (forces resize on the 3rd) ---*/
   tbl.AddColumn({"name", DataType::kString});
   tbl.AddColumn({"age",  DataType::kInt});
   tbl.AddColumn({"gpa",  DataType::kDouble});   // should invoke ResizeRows()
 
   REQUIRE(tbl.GetRows().empty());
 
   /*--- 2. add two rows -----------------------------------*/
   tbl.AddRow({"Alice", "20", "3.5"});
   tbl.AddRow({"Bob",   "22", "3.9"});
 
   auto rows = tbl.GetRows();
   REQUIRE(rows.size() == 2);
   REQUIRE(rows[0][0] == "Alice" && rows[0][1] == "20" && rows[0][2].substr(0,3) == "3.5");
   REQUIRE(rows[1][0] == "Bob");
 
   /*--- 3. add a new column with default data -------------*/
   tbl.AddColumn({"major", DataType::kString});
   rows = tbl.GetRows();
   REQUIRE(rows[0].size() == 4 && rows[0][3] == "");
   REQUIRE(rows[1][3] == "");
 
   /*--- 4. delete the 3rd column (gpa) --------------------*/
   tbl.DeleteColumnByIdx(2);
   rows = tbl.GetRows();
   REQUIRE(rows[0].size() == 3);
   REQUIRE(rows[0][2] == "");     // now “major” ended up at index 2
 
   /*--- 5. negative tests ---------------------------------*/
   REQUIRE_THROWS(tbl.DeleteColumnByIdx(42));   // OOR index
   REQUIRE_THROWS(tbl.DeleteRowById(999));      // no such id
 
   /*--- 6. delete a row -----------------------------------*/
   // id’s are 0 and 1 because table starts at 0
   tbl.DeleteRowById(0);
   REQUIRE(tbl.GetRows().size() == 1);
 
   /*--- 7. cannot delete last remaining column while rows exist ---*/
   REQUIRE_THROWS(
       [&tbl] {
         tbl.DeleteColumnByIdx(0);
         tbl.DeleteColumnByIdx(0);
         tbl.DeleteColumnByIdx(0);   // should throw when trying to remove last
       }());
 
   /*--- 8. deep copy (ctor) --------------------------------------*/
   DbTable copy(tbl);                // deep copy
   tbl.DeleteRowById(1);             // mutate original
   REQUIRE(copy.GetRows().size() == 1);      // copy unaffected
 
   /*--- 9. copy assignment ---------------------------------------*/
   DbTable other;
   other = copy;
   REQUIRE(other.GetRows().size() == copy.GetRows().size());
 
   std::cout << tbl;   // exercise operator<< (also for leak-checking)
 }
 
 /*------------------------------------------------------------*
  |  Tests for Database wrapper                                 |
  *------------------------------------------------------------*/
 void TestDatabase() {
   std::cout << ">> TestDatabase\n";
 
   Database db;
   db.CreateTable("students");
   db.CreateTable("courses");
 
   /* duplicate table */
   REQUIRE_THROWS(db.CreateTable("students"));
 
   DbTable& students = db.GetTable("students");
   students.AddColumn({"name", DataType::kString});
   students.AddColumn({"age",  DataType::kInt});
   students.AddRow({"Alice", "20"});
   students.AddRow({"Bob", "22"});
 
   /* streaming */
   std::cout << db;
 
   /* copy ctor for Database --------------------------------*/
   Database copy(db);
   REQUIRE(copy.GetTable("students").GetRows().size() == 2);
 
   /* assignment -------------------------------------------*/
   Database other;
   other = copy;
   REQUIRE(other.GetTable("students").GetRows().size() == 2);
 
   /* DropTable --------------------------------------------*/
   db.DropTable("courses");
   REQUIRE_THROWS(db.GetTable("courses"));
   REQUIRE_THROWS(db.DropTable("courses"));  // already gone
 
   /* destructor will run & free everything (valgrind/ASan) */
 }
 
 /*------------------------------------------------------------*
  |  main – run all suites                                      |
  *------------------------------------------------------------*/
 int main() {
   TestDbTable();
   TestDatabase();
 
   std::cout << "\nAll simulated tests passed 🎉\n";
   return 0;
 }



 
