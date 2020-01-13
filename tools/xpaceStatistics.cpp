#define BOOST_LOG_DYN_LINK 1

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"
#include "xpace_log_file.h"
#include "xpace_statistic.h"

void printDescription()
{
    std::cout
            << "xpaceStatistics calculates various statistical properties of log-files acquired during motion tracking."
            << std::endl;
    std::cout << std::endl << "Usage: xpaceStatistics [opts] xpace_XXX.log [more log-files]" << std::endl;
}

nlohmann::json createJSON(xpace::XPaceStatistic& statistic)
{
    using Key = xpace::XPaceStatistic::StatKey;
    nlohmann::json o;
    o["MeanEuclideanDistance"] = statistic[Key::MeanDistance];
    o["MinEuclideanDistance"] = statistic[Key::MinDistance];
    o["MaxEuclideanDistance"] = statistic[Key::MaxDistance];
    o["EuclideanDistanceStdDev"] = statistic[Key::StandardDeviation];
    o["WeightedMeanEuclideanDistance"] = statistic[Key::WeightedMeanDistance];
    return o;
}

int main(int argc, char** argv)
{
    constexpr int SUCCESS = 0;
    constexpr int ERROR = -1;
    const std::string programName("xpaceStatistics");


    using namespace xpace;
    namespace po = boost::program_options;

    std::vector<std::string> inputFiles;
    po::variables_map vm;
    po::options_description opts_description("Options");
    opts_description.add_options()
            ("help,h", "Shows this help message")
            ("input,i", po::value<std::vector<std::string> >(&inputFiles)->required(),
                    "Input XPace log-file. Note that log-files can also be specified by giving them as last arguments to xpaceStatistics.");
    po::positional_options_description opts_positional;
    opts_positional.add("input", -1);
    try {
        auto parsed = po::command_line_parser(argc, argv)
                .options(opts_description)
                .positional(opts_positional)
                .run();
        po::store(parsed, vm);

        if (vm.count("help")) {
            printDescription();
            std::cout << opts_description << std::endl;
            return SUCCESS;
        }

        po::notify(vm);

        if (vm.count("input")) {
            nlohmann::json result;
            for (const auto& f: inputFiles) {
                using Key = xpace::XPaceStatistic::StatKey;
                xpace::XpaceLogFile logFile(f);
                xpace::XPaceStatistic statistic(logFile);
                result[f] = createJSON(statistic);
            }
            std::cout << std::setw(4) << result << std::endl << std::endl;
            return SUCCESS;
        }
    }
    catch (po::required_option& e) {
        std::cerr << "Error: No input log-file specified. Try xpaceStatistics -h" << std::endl;
        return ERROR;
    }
    catch (po::unknown_option& e) {
        std::cerr << programName << ": illegal option " << e.get_option_name() << std::endl;
        return ERROR;
    }
    catch (std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return SUCCESS;
}
