//
// Created by patrick on 19.12.19.
//

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE My Test
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "xpace_log_file.h"

BOOST_AUTO_TEST_SUITE(XpaceLogFileCheckSuite)

BOOST_AUTO_TEST_CASE(ExampleLogfile)
{
    boost::filesystem::path full_path(boost::filesystem::current_path());
    auto filePath = full_path / "xpace_603.log";
    BOOST_CHECK_NO_THROW(XpaceLogFile file(filePath.string()));
}

BOOST_AUTO_TEST_SUITE_END()