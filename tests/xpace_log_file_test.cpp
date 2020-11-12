#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE XPace Library Test
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <cmath>

#include <iomanip>

#include "log_file.hpp"
#include "parser.hpp"
#include "statistic.hpp"
#include "gaussian_filter.hpp"

BOOST_AUTO_TEST_SUITE(XpaceLogFileCheckSuite)

BOOST_AUTO_TEST_CASE(ExampleLogfile)
{
	boost::filesystem::path full_path(boost::filesystem::current_path());
	auto filePath = full_path / "xpace_603.log";
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
	std::string input =
		"210528 318621399 -0.0161151 -0.0031368 -0.026798 ( 9.89364617e-06 -2.33107603e-05 -6.34317912e-05 ) 1 + 0 0 *";
	xpace::parser::motion_t result;
	bool success = xpace::parser::parseMotion(input, result);
	BOOST_CHECK_EQUAL(success, true);
}

BOOST_AUTO_TEST_CASE(ParseFile)
{
	boost::filesystem::path full_path(boost::filesystem::current_path());
	auto filePath = full_path / "xpace_603.log";
	xpace::XpaceLogFile logFile(filePath.string());
	BOOST_CHECK_EQUAL(logFile.getNumberOfMotions(), 43647);
	std::cout << logFile.getInitialPosition() << std::endl;
	std::cout << "Number of motions: " << logFile.getNumberOfMotions() << std::endl;
}

BOOST_AUTO_TEST_CASE(WeirdLine)
{
	// The weird thing was that I did not realize that the time and frame values might not fit into an int.
	// They are unsigned ints now.
	std::string line =
		"200341 2687970876 0.02327 0.045986 0.067551 ( 3.85996755e-05 -2.06852255e-05 -1.25979786e-05 ) -1 + 0 0 *\r";
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

	auto result = motion.applyToPose(initialPosition);
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

BOOST_AUTO_TEST_CASE(XPaceStatistics)
{
	boost::filesystem::path full_path(boost::filesystem::current_path());
	auto filePath = full_path / "xpace_603.log";
	xpace::XpaceLogFile logFile(filePath.string());
	xpace::XPaceStatistic stats(logFile);

}

BOOST_AUTO_TEST_CASE(LowpassFilter)
{
	std::vector<double> gaussianSigma2{3.8472993133531976e-23, 5.038967697149989e-21,
									   5.139886785834441e-19, 4.083117815834762e-17,
									   2.526135541768438e-15, 1.2171602665145056e-13,
									   4.5673602041822984e-12, 1.3347783073814265e-10,
									   3.037941424911644e-9, 5.384880021271641e-8,
									   7.433597573671492e-7, 7.99187055345274e-6,
									   0.00006691511288244271, 0.00043634134752288024,
									   0.0022159242059690046, 0.008764150246784272,
									   0.026995483256594024, 0.06475879783294586,
									   0.12098536225957167, 0.17603266338214973,
									   0.19947114020071632, 0.17603266338214973,
									   0.12098536225957167, 0.06475879783294586,
									   0.026995483256594024, 0.008764150246784272,
									   0.0022159242059690046, 0.00043634134752288024,
									   0.00006691511288244271, 7.99187055345274e-6,
									   7.433597573671492e-7, 5.384880021271641e-8,
									   3.037941424911644e-9, 1.3347783073814265e-10,
									   4.5673602041822984e-12, 1.2171602665145056e-13,
									   2.526135541768438e-15, 4.083117815834762e-17,
									   5.139886785834441e-19, 5.038967697149989e-21,
									   3.8472993133531976e-23};
	std::vector<double> gaussianSigma20{0.01209853622595717,0.012702952823459453,
										0.01330426249493774,0.013899244306549825,
										0.014484577638074135,0.015056871607740224,
										0.015612696668338064,0.016148617983395723,
										0.01666123014458999,0.017147192750969195,
										0.017603266338214976,0.018026348123082396,
										0.01841350701516617,0.018762017345846902,
										0.01906939077302621,0.019333405840142464,
										0.0195521346987728,0.01972396654539445,
										0.019847627373850596,0.019922195704738202,
										0.019947114020071637,0.019922195704738202,
										0.019847627373850596,0.01972396654539445,
										0.0195521346987728,0.019333405840142464,
										0.01906939077302621,0.018762017345846902,
										0.01841350701516617,0.018026348123082396,
										0.017603266338214976,0.017147192750969195,
										0.01666123014458999,0.016148617983395723,
										0.015612696668338064,0.015056871607740224,
										0.014484577638074135,0.013899244306549825,
										0.01330426249493774,0.012702952823459453,
										0.01209853622595717};
	std::vector<double> in{0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
						   0., 0., 0., 0., 0., 1., 0., 0., 0., 0., 0., 0., 0.,
						   0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
	xpace::GaussianFilter filter2(2.0);
	auto output = filter2.filter(in);
	double err = 0.0;
	for (int i = 0; i < gaussianSigma2.size(); ++i) {
		auto currErr = std::abs(gaussianSigma2[i] - output[i]);
		BOOST_CHECK(currErr < 0.0005);
		err += currErr;
	}
	BOOST_CHECK(err < 0.005);

	xpace::GaussianFilter filter20(20.0);
	output = filter20.filter(in);
	err = 0.0;
	for (int i = 0; i < gaussianSigma20.size(); ++i) {
		auto currErr = std::abs(gaussianSigma20[i] - output[i]);
		BOOST_CHECK(currErr < 0.0001);
		err += currErr;
	}
	BOOST_CHECK(err < 0.0003);
}

BOOST_AUTO_TEST_SUITE_END()
