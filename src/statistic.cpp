#include "statistic.hpp"
#include <vector>
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

	// Mean time difference between measurement points.
	// Using this is not quite correct since the log-file will contain invalid measurements and the
	// sequence of correct measures is in general not uniformly sampled.
	// However, for a simple analysis like this, resampling everything seems overkill.
	double meanDeltaT = (motions.end()->time - motions.begin()->time) / (1.0e-6 * length);
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

	// Speed: Note that this is squared speed for later summation and square-rooting.
	// Kerrin's wish was to weight the contributions because k-space center is more important. The assumption is that
	// the k-space center is scanned in the middle of the scan. Therefore, we make a simple ramp-up/down that peaks
	// in the middle of the list of distances_.
	speed_.resize(length);
	speed_[0] = 0.0;
	for (int i = 1; i < length; ++i) {
		auto t = times_[i] - times_[i - 1];
		auto vecDiff = positions_[i] - positions_[i-1];
		auto angleDiff = angles_[i] - angles_[i-1];
		speed_[i] = (vecDiff.sqNorm() + angleDiff.sqNorm()) / (t * t);
	}

	// Integrated speed
	stats_[IntegratedSpeed] = std::sqrt(std::accumulate(speed_.begin(), speed_.end(), 0.0));

	// Partition weighted integrated speed
	auto count = 0;
	auto weightingFunctionSpeed = [&](double current, double val)
	{
		return current + val * (1.0 - 2.0 * std::abs(count++ / (speed_.size() - 1.0) - 0.5));
	};
	stats_[PartitionWeightedIntegratedSpeed] =
		2.0 / (speed_.size() - 1.0) * std::accumulate(speed_.begin(), speed_.end(), 0.0, weightingFunctionSpeed);


	// Net Motion
	// Simply the last motion in the logfile
	// Todo: Make this the real translation and rotations
	lastMotion_ = motions.back();

	auto mean = std::accumulate(distances_.begin(), distances_.end(), 0.0) / length;
	auto minMax = std::minmax_element(distances_.begin(), distances_.end());
	stats_[MeanDistance] = mean;
	stats_[MinDistance] = *minMax.first;
	stats_[MaxDistance] = *minMax.second;

	auto stdDevFunc = [=](double current, double val) -> double
	{
		return current + (val - mean) * (val - mean);
	};
	auto stdDev = std::sqrt(std::accumulate(distances_.begin(), distances_.end(), 0.0, stdDevFunc) / (length - 1.0));
	stats_[StandardDeviation] = stdDev;

	count = 0;
	auto weightingFunctionDistances = [&](double current, double val)
	{
		return current + val * (1.0 - 2.0 * std::abs(count++ / (length - 1.0) - 0.5));
	};
	stats_[WeightedMeanDistance] =
		2.0 / (length - 1.0) * std::accumulate(distances_.begin(), distances_.end(), 0.0, weightingFunctionDistances);

}

std::string XPaceStatistic::getLogFilename()
{
	return file_.getFilename();
}

}