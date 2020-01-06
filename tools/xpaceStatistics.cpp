#define BOOST_LOG_DYN_LINK 1

#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>
#include "xpace_log_file.h"

int main(int argc, char** argv)
{
    constexpr size_t SUCCESS = 0;
    constexpr size_t ERROR = -1;
    using namespace xpace;
    namespace po = boost::program_options;
    std::string filename;
    po::variables_map vm;
    po::options_description desc{"Options"};
    desc.add_options()
            ("input,i", po::value<std::string>(&filename)->required(), "Input log-files");
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("input")) {
            std::cout << filename << std::endl;
            return SUCCESS;
        }
    }
    catch (po::required_option& e) {
        BOOST_LOG_TRIVIAL(error) << "Please specify input file with -i file or --input file";
        return ERROR;
    }
    return SUCCESS;
}
