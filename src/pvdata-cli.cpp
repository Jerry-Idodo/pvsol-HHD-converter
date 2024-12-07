#include "pvdata-cli.hpp"
#include "pvdata.hpp"
#include <iostream>
#include <ostream>
#include <string>

int validate_inputs(char **argv, HHData::Intervals& interval)
{
    try {
        int interval_int = std::stoi(std::string(argv[3]));
        interval = static_cast<HHData::Intervals>(interval_int);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return INPUT_ERROR; // Return an error code
    } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << std::endl;
        return INPUT_ERROR; // Return an error code
    }
    return 0;
}

void print_usage()
{
    std::cout << "hh-pvdata: converts pvsol generation data with various time intervals to a half-hourly table.\n"
    << "usage: hh-pvdata PVSOL_FILE OUTPUT_FILE INTERVAL\n"
    << "INTERVALs available: 1, 5, 15, 30 and 60 minutes" << std::endl;
}
