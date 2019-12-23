//
// Created by patrick on 19.12.19.
//

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE XPace Library Test
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "xpace_log_file.h"
#include "xpace_parser.h"

BOOST_AUTO_TEST_SUITE(XpaceLogFileCheckSuite)

BOOST_AUTO_TEST_CASE(ExampleLogfile)
{
    boost::filesystem::path full_path(boost::filesystem::current_path());
    auto filePath = full_path / "xpace_603.log";
    BOOST_CHECK_NO_THROW(xpace::XpaceLogFile file(filePath.string()));
}

BOOST_AUTO_TEST_CASE(ParseInitialMotion){
    std::string input = "@11.556 142.285 190.512 ( 0.648970242 0.0581997974 -0.0450109512 ) -0.757247959";
    xpace::parser::initial_pose_t result;
    bool success = xpace::parser::parseInitialPose(input, result);
    BOOST_CHECK_EQUAL(success, true);
}

BOOST_AUTO_TEST_CASE(ParseMotion){
    std::string input = "210528 318621399 -0.0161151 -0.0031368 -0.026798 ( 9.89364617e-06 -2.33107603e-05 -6.34317912e-05 ) 1 + 0 0 *";
    xpace::parser::motion_t result;
    bool success = xpace::parser::parseMotion(input, result);
    BOOST_CHECK_EQUAL(success, true);
}

BOOST_AUTO_TEST_CASE(ParseFile){
    boost::filesystem::path full_path(boost::filesystem::current_path());
    auto filePath = full_path / "xpace_603.log";
    xpace::XpaceLogFile logFile(filePath.string());
    BOOST_CHECK_EQUAL(logFile.getNumberOfMotions(),43647);
    std::cout << logFile.getInitialPosition() << std::endl;
    std::cout << "Number of motions: " << logFile.getNumberOfMotions() << std::endl;
}

BOOST_AUTO_TEST_SUITE_END()