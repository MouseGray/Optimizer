#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include "sma.h"
#include "macd.h"

enum class Algorithm
{
    SMA,
    SMAPair,
    MACD
};

struct Config
{
    std::string                      fileName;
    std::vector<std::pair<int, int>> periods;
    double                           fee = 0.0;
    bool                             verbose = false;
    Algorithm                        algorithm = Algorithm::SMA;
};

static void printHelp()
{
    std::cout << "SMA [options] <filename>\n"
              << "-p\t\t- Period (begin-end)\n"
              << "-v\t\t- Verbose\n"
              << "-f\t\t- Fee (from 0.0 to 1.0)\n"
              << "-a\t\t- Algorithm (SMA|SMAPair|MACD)\n"
              << "--help\t\t- Help\n";
}

static bool parseArguments(Config& config, int argc, char** argv)
{
    if(argc < 2 || !std::strcmp(argv[1], "--help"))
    {
        printHelp();
        return false;
    }

    for(int i = 1; i < argc - 1; ++i)
    {
        if(!std::strcmp(argv[i], "-p"))
        {
            if(i == (argc - 1) || (i + 1) == (argc - 1))
            {
                printHelp();
                return false;
            }

            std::string_view period_string{argv[i + 1]};

            auto split_pos = period_string.find('-');

            if(split_pos == period_string.npos)
            {
                printHelp();
                return false;
            }

            auto begin = std::stoi(
                        std::string{period_string.substr(0, split_pos)}),
                 end   = std::stoi(
                        std::string{period_string.substr(split_pos + 1)});

            config.periods.emplace_back(begin, end);
        }
        else if(!std::strcmp(argv[i], "-f"))
        {
            if(i == (argc - 1) || (i + 1) == (argc - 1))
            {
                printHelp();
                return false;
            }

            config.fee = std::stod(argv[i + 1]);
        }
        else if(!std::strcmp(argv[i], "-v"))
        {
            if(i == (argc - 1))
            {
                printHelp();
                return false;
            }

            config.verbose = true;
        }
        else if(!std::strcmp(argv[i], "-a"))
        {
            if(i == (argc - 1) || (i + 1) == (argc - 1))
            {
                printHelp();
                return false;
            }

            if(!std::strcmp(argv[i + 1], "SMA"))
                config.algorithm = Algorithm::SMA;
            else if(!std::strcmp(argv[i + 1], "SMAPair"))
                config.algorithm = Algorithm::SMAPair;
            else if(!std::strcmp(argv[i + 1], "MACD"))
                config.algorithm = Algorithm::MACD;
            else
            {
                printHelp();
                return false;
            }
        }
    }

    config.fileName = argv[argc - 1];

    return true;
}

int main(int argc, char** argv)
{
    Config config;
    try
    {
        if(!parseArguments(config, argc, argv))
            return 0;

        auto series = loadSeries(config.fileName);

        double           evaluation;
        std::vector<int> period;

        if(config.algorithm == Algorithm::SMA)
        {
            std::tie(evaluation, period) =
                    optimizeIndicator<SMAIndicator, 1>(series, config.periods);
        }
        else if(config.algorithm == Algorithm::SMAPair)
        {
            std::tie(evaluation, period) =
                    optimizeIndicator<SMAPairIndicator, 2>(series,
                                                           config.periods);
        }
        else if(config.algorithm == Algorithm::MACD)
        {
            std::tie(evaluation, period) =
                    optimizeIndicator<MACDIndicator, 3>(series, config.periods);
        }

        std::cout << "Best period: " << toPeriodText(period) << "\n"
                  << "Evaluation: "  << evaluation << "\n";
    }
    catch(const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
