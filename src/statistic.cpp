#include "statistic.hpp"
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "gaussian_filter.hpp"

namespace xpace
{

XPaceStatistic::XPaceStatistic(const XpaceLogFile &file)
	: file_(file), initialPose_(file.getInitialPosition())
{
	if (file_.getNumberOfMotions() > 0) {
		calculateStatistics();
	}
	else {
		throw std::runtime_error("No motions in logfile.");
	}
}

void XPaceStatistic::calculateStatistics()
{
	auto motions = file_.getMotions();
	auto length = file_.getNumberOfMotions();

	if (length < 5) {
		std::stringstream err;
		err << "Only " << length << " motion-measures available. That's certainly not enough to calculate sensible statistics.";
		throw std::runtime_error(err.str());
	}

	long double timeDeltaMean = static_cast<long double>(motions.back().time - motions.front().time) / (1.0e6L * (length-1.0L));
	distances_.resize(length);

	times_.resize(length);
	positions_.resize(length);
	angles_.resize(length);

	Vector initPos(initialPose_.t);
	EulerAngle initAngle(initialPose_.q);
	auto absPositions = file_.getAbsolutePositions();
	for (int i = 0; i < length; ++i) {
		times_[i] = static_cast<double>(absPositions[i].time - absPositions[0].time) * 1.0e-6;
		positions_[i] = absPositions[i].t - initPos;
		angles_[i] = EulerAngle(absPositions[i].q) - initAngle;
		distances_[i] = std::sqrt(positions_[i].sqNorm() + angles_[i].sqNorm());
	}

	speed_.resize(length);
	speed_[0] = 0.0;
	long double timeDeltaStdDev = 0.0;
	for (int i = 1; i < length; ++i) {
		auto t = times_[i] - times_[i - 1];
		auto vecDiff = positions_[i] - positions_[i-1];
		auto angleDiff = angles_[i] - angles_[i-1];
		speed_[i] = std::sqrt(vecDiff.sqNorm() + angleDiff.sqNorm()) / t;
		timeDeltaStdDev += (t - timeDeltaMean) * (t - timeDeltaMean);
	}

	// We needed long double to be sure the micro-seconds time values don't overflow.
	// It should be save to cast the mean and the stddev back to double here.
	stats_[TimeDeltaMean] = static_cast<double>(timeDeltaMean);
	stats_[TimeDeltaStandardDeviation] = std::sqrt(static_cast<double>(timeDeltaStdDev)/(times_.size()-1.0));
	// Integrated speed
	stats_[SpeedIntegral] = std::accumulate(speed_.begin(), speed_.end(), 0.0);

	// Partition weighted integrated speed
	// Kerrin's wish was to weight the contributions because k-space center is more important. The assumption is that
	// the k-space center is scanned in the middle of the scan. Therefore, we make a simple ramp-up/down that peaks
	// in the middle.
	auto count = 0;
	auto weightingFunctionSpeed = [&](double current, double val)
	{
		return current + val * (1.0 - 2.0 * std::abs(count++ / (speed_.size() - 1.0) - 0.5));
	};
	stats_[SpeedWeightedIntegral] =
		2.0 / (speed_.size() - 1.0) * std::accumulate(speed_.begin(), speed_.end(), 0.0, weightingFunctionSpeed);


	// Net Motion
	// Simply the last motion in the logfile
	// Todo: Make this the real translation and rotations
	lastMotion_ = motions.back();

	auto mean = std::accumulate(distances_.begin(), distances_.end(), 0.0) / length;
	auto minMax = std::minmax_element(distances_.begin(), distances_.end());
	stats_[DistanceMean] = mean;
	stats_[DistanceMin] = *minMax.first;
	stats_[DistanceMax] = *minMax.second;

	auto stdDevFunc = [=](double current, double val) -> double
	{
		return current + (val - mean) * (val - mean);
	};
	auto stdDev = std::sqrt(std::accumulate(distances_.begin(), distances_.end(), 0.0, stdDevFunc) / (length - 1.0));
	stats_[DistanceStandardDeviation] = stdDev;

	count = 0;
	auto weightingFunctionDistances = [&](double current, double val)
	{
		return current + val * (1.0 - 2.0 * std::abs(count++ / (length - 1.0) - 0.5));
	};
	stats_[DistanceWeightedMean] =
		2.0 / (length - 1.0) * std::accumulate(distances_.begin(), distances_.end(), 0.0, weightingFunctionDistances);

}

std::string XPaceStatistic::getLogFilename()
{
	return file_.getFilename();
}

}