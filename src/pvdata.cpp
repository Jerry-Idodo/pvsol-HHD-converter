#include "pvdata.hpp"
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <numeric>
#include <string>
#include <vector>

int HHData::transform_csv(const std::string& pvfile,  HHData::Intervals time_interval)
{
    filename = pvfile;
    interval = time_interval;
    start_year = "2025";
    std::vector<std::vector<std::string>> raw_values;

    if (load_data(raw_values) != 0) {
        std::cerr << "error loading file " << filename << "\n";
        return RUNTIME_ERROR;
    }
    if (gen_tables(raw_values) != 0) {
        std::cerr << "error creating tables\n";
        return RUNTIME_ERROR;
    }

    return 0;
}

int HHData::load_data(std::vector<std::vector<std::string>>& raw_values)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return RUNTIME_ERROR;
    }
    std::string line;
    std::string gen_header = "\"PV energy (DC) \"";
    std::getline(file, line);
    int data_col = get_generation_header(line, gen_header);
    if (data_col == 0) {
        std::cout << "Could not find PV energy (DC) \n";
        return RUNTIME_ERROR;
    }

    while (std::getline(file, line)) {
        std::vector<std::string> tmp = split_line(line);
        if (tmp.size() < data_col){
            continue;
        }
        if (tmp[0] == "01.01. 00:00") {
            raw_values.push_back(gen_raw_values(tmp, data_col));
            break;
        }
    }

    while (std::getline(file, line)) {
        std::vector<std::string> tmp = split_line(line);
        raw_values.push_back(gen_raw_values(tmp, data_col));
    }
    return 0;
}

int HHData::get_generation_header(const std::string& line, const std::string& gen_header)
{
    std::vector<std::string> tmp = split_line(line);
    for (int i = 0; i < tmp.size(); i++) {
        if (tmp[i] == gen_header) {
            return i;
        }
    }
    return 0;
}

std::vector<std::string> HHData::gen_raw_values(const std::vector<std::string>& tokens, int data_col)
{
    std::vector<std::string> values = split_datetime(tokens[0], start_year);
    values.push_back(tokens[data_col]);
    return values;
}

std::vector<std::string> HHData::split_line(const std::string& line, char delim)
{
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream buff_stream(line);

    while (getline(buff_stream, token, delim)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> HHData::split_datetime(const std::string& token, const std::string& year)
{
    std::vector<std::string> tmp = split_line(token, ' ');
    std::vector<std::string> day_month = split_line(tmp[0], '.');
    std::string year_month_day = year + "-" + day_month[1] + "-" + day_month[0];
    return std::vector{year_month_day, tmp[1]};
}

int HHData::gen_tables(std::vector<std::vector<std::string>>& raw_values)
{
    switch (interval) {
    case Intervals::M_1:
    case Intervals::M_5:
    case Intervals::M_15:
    case Intervals::M_30:
        combine_generation_values(raw_values);
        break;
    case Intervals::M_60:
        split_generation_values(raw_values);
        break;
    }
    return 0;
}

int HHData::combine_generation_values(const std::vector<std::vector<std::string>>& raw_values)
{
    const int half_hour = 30;
    const int half_hours = 48;
    const int days = 365;
    uint8_t steps = half_hour / static_cast<uint8_t>(interval);
    uint16_t day_length = half_hours * steps;
    uint16_t counter = 0;
    if (raw_values.size() != day_length * days) {
        std::cout << "Unexpected data size\n";
        return RUNTIME_ERROR;
    }

    for (int i = 0; i < days; i ++) {
        std::chrono::year_month_day date = get_date(raw_values[counter][0]);
        std::vector<double> hh_values;
        for (int j = 0; j < half_hours; j++) {
            double tmp = 0;
            for (int k = 0; k < steps; k++) {
                tmp += std::stod(raw_values[counter][2]);
                counter++;
            }
            hh_values.push_back(tmp);
        }
        data.insert({date, hh_values});
    }
    return 0;
}

int HHData::split_generation_values(const std::vector<std::vector<std::string>>& raw_values)
{
    const int half_hour = 30;
    const int half_hours = 48;
    const int days = 365;
    uint8_t steps = static_cast<uint8_t>(interval) / half_hour;
    uint16_t day_length = half_hours / steps;
    uint16_t counter = 0;
    if (raw_values.size() != day_length * days) {
        std::cout << "Unexpected data size\n";
        return RUNTIME_ERROR;
    }

    for (int i = 0; i < days; i ++) {
        std::chrono::year_month_day date = get_date(raw_values[counter][0]);
        std::vector<double> hh_values;
        for (int j = 0; j < half_hours; j++) {
            double tmp = 0;
            for (int k = 0; k < steps; k++) {
                tmp += std::stod(raw_values[counter][2]) / steps;
            }
            hh_values.push_back(tmp);
            counter++;
        }
        data.insert({date, hh_values});
    }
    return 0;
}

std::chrono::year_month_day HHData::get_date(const std::string& date_str)
{
    std::istringstream iss(date_str);
    int year, month, day;
    char delim;

    iss >> year >> delim >> month >> delim >> day;

    return {std::chrono::year(year), std::chrono::month(month), std::chrono::day(day)};
}

int HHData::save_data(std::string& outfile)
{
    std::ofstream file(outfile);

    if (!file.is_open()) {
        std::cerr << "Error saving file: " << outfile << "\n";
        return RUNTIME_ERROR;
    }
    print_headers(file);
    for (const auto& pair : data) {
        file << pair.first << ",";
        file << std::fixed << std::setprecision(2)
        << std::accumulate(pair.second.begin(), pair.second.end(), 0.0);
        for (const auto& value : pair.second) {
            file << "," << value;
        }
        file << "\n";
    }
    return 0;
}

void HHData::print_headers(std::ofstream& file)
{
    file << "Date,Total";
    std::chrono::duration<double, std::ratio<60>> time(0.0);
    const int half_hours = 48;
    for (int i = 0; i < half_hours; i++) {
        file << "," << std::chrono::hh_mm_ss{time};
        time += std::chrono::minutes{30};
    }
    file << "\n";
}
