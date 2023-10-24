#include "series.h"

#include <fstream>

Series loadSeries(const std::string& file_name)
{
    double value;

    std::ifstream in{file_name};

    if(!in.is_open())
        throw std::invalid_argument{"File " + file_name + " not found"};

    Series series;

    while(in >> value)
        series.emplace_back(value);

    return series;
}
