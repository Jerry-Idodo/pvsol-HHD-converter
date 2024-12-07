#include "pvdata.hpp"
#include "pvdata-cli.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 4) {
        print_usage();
        return USAGE_ERROR;
    }
    HHData::Intervals interval;
    if (validate_inputs(argv, interval) != 0) {
        print_usage();
        return INPUT_ERROR;
    }

    HHData PVData;
    std::string pv_filename = argv[1];
    std::string hh_filename = argv[2];
    int ret = PVData.transform_csv(pv_filename, interval);
    if (ret != 0)
        return ret;
    ret = PVData.save_data(hh_filename);
    return ret;
}
