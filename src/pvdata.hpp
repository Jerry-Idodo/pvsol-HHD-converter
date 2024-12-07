#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <chrono>

class HHData {
public:
    enum class Intervals : uint8_t {
        M_1 = 1,
        M_5 = 5,
        M_15 = 15,
        M_30 = 30,
        M_60 = 60
    };
    int transform_csv(const std::string& pvfile, HHData::Intervals interval);
    int save_data(std::string& outfile);

private:
    std::string filename;
    HHData::Intervals interval;
    std::string start_year;
    std::map <std::chrono::year_month_day, std::vector<double>> data;

    int load_data(std::vector<std::vector<std::string>>& raw_values);
    int gen_tables(std::vector<std::vector<std::string>>& raw_values);

    static int get_generation_header(const std::string& line, const std::string& gen_header);
    std::vector<std::string> gen_raw_values(const std::vector<std::string>& tokens, int data_col);
    static std::vector<std::string> split_line(const std::string& line, char delim = ',');
    static std::vector<std::string> split_datetime(const std::string& token, const std::string& year);
    int combine_generation_values(const std::vector<std::vector<std::string>>& raw_values);
    int split_generation_values(const std::vector<std::vector<std::string>>& raw_values);
    static std::chrono::year_month_day get_date(const std::string& date_str);
    static void print_headers(std::ofstream& file);
};

enum Exit_Code {
    NONE = 0, USAGE_ERROR, INPUT_ERROR, RUNTIME_ERROR
};
