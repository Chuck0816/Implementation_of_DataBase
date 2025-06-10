#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "db.hpp"
#include "db_table.hpp"

// Function to export a table to a CSV file
void ExportTableToCSV(const DbTable& table, const std::string& filename) {
    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Failed to create " << filename << std::endl;
        return;
    }

    // Write the CSV header (column names)
    const auto& column_descriptions = table.GetColumnDescriptions();
    for (size_t i = 0; i < column_descriptions.size(); ++i) {
        csv_file << column_descriptions[i].first; // Access column name
        if (i < column_descriptions.size() - 1) {
            csv_file << ",";
        }
    }
    csv_file << "\n";

    // Write the data rows
    for (const auto& row : table.GetRows()) {
        for (size_t i = 0; i < row.size(); ++i) {
            csv_file << row[i];
            if (i < row.size() - 1) {
                csv_file << ",";
            }
        }
        csv_file << "\n";
    }

    csv_file.close();
    std::cout << "Exported table to " << filename << std::endl;
}

// Function to update the stats interactively (using Option 2 - Copy the row)
// Function to update the stats interactively (using Option 2 - Copy the row)
// Function to update the stats interactively (using Option 2 - Copy the row)
// ──────────────────────────────────────────────────────────────
// Abort-all-columns version of UpdateStats
// ──────────────────────────────────────────────────────────────


int main() {
    try {
        Database db;

        // Create and manipulate the first table (Team Stats)
        db.CreateTable("league_data");
        DbTable& stats_ = db.GetTable("league_data");
        stats_.AddColumn({"Team_Name", DataType::kString});
        stats_.AddColumn({"Ranking", DataType::kInt});
        stats_.AddColumn({"Goal", DataType::kInt});
        stats_.AddColumn({"Average_shots_per_game", DataType::kDouble});
        stats_.AddColumn({"Average_shots_on_target_per_game", DataType::kDouble});
        stats_.AddColumn({"Average_passes_per_game", DataType::kInt});
        stats_.AddColumn({"Probability_of_successful_passes_per_game", DataType::kDouble});
        stats_.AddColumn({"Key_passes", DataType::kInt});
        stats_.AddColumn({"Goals_conceded", DataType::kInt});
        stats_.AddColumn({"Tackles", DataType::kInt});
        stats_.AddColumn({"Clearance", DataType::kInt});

        stats_.AddRow({"Shanghai ShenHua FC", "1", "23", "17.6", "5.9", "415", "0.8", "153", "12", "185", "275"});
        stats_.AddRow({"Chengdu RongCheng FC", "2", "18", "18.3", "6.9", "442", "0.79", "138", "7", "174", "246"});
        stats_.AddRow({"Beijing Guoan FC","3", "24", "13.8", "5.6", "543", "0.86", "104", "11", "152", "301"});
        stats_.AddRow({"Shandong Taishan FC", "5", "23", "16.7", "5.5", "393", "0.78", "141", "20", "180", "308"});
        stats_.AddRow({"Shanghai Port FC","4", "22", "15.9", "6.0", "480", "0.84", "107", "14", "200", "254"});
        stats_.AddRow({"Qingdao West Coast FC","6", "19", "11.1", "3.7", "376", "0.72", "81", "17", "194", "241"});
        stats_.AddRow({"Tianjin jinmen Tiger FC","7", "18", "14", "4.9", "380", "0.76", "104", "18", "197", "383"});
        stats_.AddRow({"Zhejiang FC","8", "20", "10.8", "3.6", "459", "0.82", "86", "16", "165", "315"});
        stats_.AddRow({"Dalian Yingbo FC","9", "11", "11.6", "3.1", "360", "0.73", "89", "16", "203", "296"});
        stats_.AddRow({"Meizhuo Hakka FC","10", "20", "12.8", "3.8", "371", "0.76", "96", "22", "193", "325"});
        stats_.AddRow({"Yunnan Yukun FC","11", "16", "11.4", "3.5", "312", "0.70", "91", "21", "203", "320"});
        stats_.AddRow({"Wuhan Tree Towns FC","12", "13", "13.5", "4.9", "376", "0.76", "101", "22", "203", "309"});
        stats_.AddRow({"Shenzhen Peng City FC","13", "11", "9.8", "2.9", "378", "0.77", "79", "22", "227", "279"});
        stats_.AddRow({"Henan FC","14", "14", "11.4", "4.2", "365", "0.77", "83", "18", "169", "270"});
        stats_.AddRow({"Qingdao Hainiu FC","15", "12", "9.8", "2.9", "366", "0.75", "72", "18", "226", "468"});
        stats_.AddRow({"Changchun Yatai FC","16", "10", "9.1", "3.1", "359", "0.71", "68", "22", "200", "435"});
        std::cout << "Initial stats recorded.\n" << std::endl;


        // Export the updated table to a CSV
        ExportTableToCSV(stats_, "league_data.csv");



/*-----------------------------------------------------------------------------------------------------*/
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
