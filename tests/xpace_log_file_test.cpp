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
    auto filePath = full_path/"xpace_603.log";
    BOOST_CHECK_NO_THROW(xpace::XpaceLogFile file(filePath.string()));
}

BOOST_AUTO_TEST_CASE(ParseInitialMotion)
{
    std::string input = "@11.556 142.285 190.512 ( 0.648970242 0.0581997974 -0.0450109512 ) -0.757247959";
    xpace::parser::initial_pose_t result;
    bool success = xpace::parser::parseInitialPose(input, result);
    BOOST_CHECK_EQUAL(success, true);
}

BOOST_AUTO_TEST_CASE(ParseMotion)
{
    std::string input = "210528 318621399 -0.0161151 -0.0031368 -0.026798 ( 9.89364617e-06 -2.33107603e-05 -6.34317912e-05 ) 1 + 0 0 *";
    xpace::parser::motion_t result;
    bool success = xpace::parser::parseMotion(input, result);
    BOOST_CHECK_EQUAL(success, true);
}

BOOST_AUTO_TEST_CASE(ParseFile)
{
    boost::filesystem::path full_path(boost::filesystem::current_path());
    auto filePath = full_path/"xpace_603.log";
    xpace::XpaceLogFile logFile(filePath.string());
    BOOST_CHECK_EQUAL(logFile.getNumberOfMotions(), 43647);
    std::cout << logFile.getInitialPosition() << std::endl;
    std::cout << "Number of motions: " << logFile.getNumberOfMotions() << std::endl;
}

BOOST_AUTO_TEST_CASE(WeirdLine)
{
    // The weird thing was that I did not realize that the time and frame values might not fit into an int.
    // They are unsigned ints now.
    std::string line = "200341 2687970876 0.02327 0.045986 0.067551 ( 3.85996755e-05 -2.06852255e-05 -1.25979786e-05 ) -1 + 0 0 *\r";
    xpace::parser::motion_t motion;
    bool result = xpace::parser::parseMotion(line, motion);
    BOOST_CHECK(result);
}

BOOST_AUTO_TEST_CASE(ApplyTransform)
{
    // Test case file xpace_603.log
    // 211563 330845821 -0.023077 0.115193 -0.134609 ( -0.000302374596 -0.000111433073 -7.44677818e-05 ) 0.99999994
    xpace::Motion motion(
            xpace::parser::motion_t{
                    211563,
                    330845821,
                    -0.023077,
                    0.115193,
                    -0.134609,
                    -0.000302374596,
                    -0.000111433073,
                    -7.44677818e-05,
                    0.99999994
            }
    );
    xpace::InitialPosition initialPosition(
            xpace::parser::initial_pose_t{
                    11.556,
                    142.285,
                    190.512,
                    0.648970242,
                    0.0581997974,
                    -0.0450109512,
                    -0.757247959
            }
    );

    auto result = motion.toAbsoluteCoordinates(initialPosition);
    BOOST_CHECK_EQUAL(result.time, motion.time);
    BOOST_CHECK_EQUAL(result.frame, motion.frame);
    BOOST_CHECK_CLOSE(result.t.x, 0.115116732530409e2, 1e-05);
    BOOST_CHECK_CLOSE(result.t.y, 1.425136587068272e2, 1e-05);
    BOOST_CHECK_CLOSE(result.t.z, 1.902938810000333e2, 1e-05);
    BOOST_CHECK_CLOSE(result.q.qr, -0.757048547933802, 1e-05);
    BOOST_CHECK_CLOSE(result.q.qi, 0.649208525325884, 1e-05);
    BOOST_CHECK_CLOSE(result.q.qj, 0.058222238832546, 1e-05);
    BOOST_CHECK_CLOSE(result.q.qk, -0.044899839315438, 1e-05);
}

BOOST_AUTO_TEST_SUITE_END()
