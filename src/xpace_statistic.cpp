#include "xpace_statistic.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace xpace
{

XPaceStatistic::XPaceStatistic(const XpaceLogFile &file)
	: file_(file), initialPose_(file.getInitialPosition())
{
	if (file_.getNumberOfMotions() > 0) {
		calculateStatistics();
	} else {
		throw std::runtime_error("No motions in logfile.");
	}
}

void XPaceStatistic::calculateStatistics()
{
	auto motions = file_.getRelativeMotions();
	auto length = file_.getNumberOfMotions();

	// Net Motion
	// Simply the last motion in the logfile
	lastMotion_ = motions.back();

	// RMS of all motions
	rmsMotions_.resize(motions.size());
	std::transform(motions.begin(), motions.end(), rmsMotions_.begin(),
				[](Motion m) -> double { return m.euclideanDistance(); });

	// Speed as the derivative of RMS
	for (int i = 1; i < rmsMotions_.size(); ++i) {
		speed_.emplace_back(rmsMotions_[i] - rmsMotions_[i-1]);
	}

	// Integrated speed
	stats_[IntegratedSpeed] = std::accumulate(speed_.begin(), speed_.end(), 0.0);

	// Partition weighted integrated speed
	auto count = 0;
	auto weightingFunctionSpeed = [&](double current, double val)
	{
		return current + val * (1.0 - 2.0 * std::abs(count++ / (speed_.size() - 1.0) - 0.5));
	};
	stats_[PartitionWeightedIntegratedSpeed] =
		2.0 / (speed_.size() - 1.0) * std::accumulate(speed_.begin(), speed_.end(), 0.0, weightingFunctionSpeed);


	// Each tracked motion is transformed into scanner coordinates which incorporates both the shift t and the
	// rotational part q. Then we subtract the initial position from the resulting point to get the vector of how
	// much the tracked motion moves the mouthpiece from its initial position.
	// This appears to me to be a more useful insight than looking purely at the motions.
	relativePositions_ = std::vector<Vector>();
	for (const auto &m: motions) {
		relativePositions_.push_back(
			m.applyToPose(initialPose_).t - initialPose_.t
		);
	}

	// Statistics are calculated on the basis of the Euclidean distance of the deviation from the initial position.
	auto euclideanDistance = [](Vector b) -> double
	{
		return std::sqrt(b.x * b.x + b.y * b.y + b.z * b.z);
	};

	std::vector<double> distances(length);
	std::transform(relativePositions_.begin(), relativePositions_.end(), distances.begin(), euclideanDistance);
	auto mean = std::accumulate(distances.begin(), distances.end(), 0.0) / length;
	stats_[MeanDistance] = mean;

	auto minmax = std::minmax_element(distances.begin(), distances.end());
	stats_[MinDistance] = *minmax.first;
	stats_[MaxDistance] = *minmax.second;

	auto stdDevFunc = [=](double current, double val) -> double
	{
		return current + (val - mean) * (val - mean);
	};
	auto stdDev = std::sqrt(std::accumulate(distances.begin(), distances.end(), 0.0, stdDevFunc) / (length - 1.0));
	stats_[StandardDeviation] = stdDev;

	// Kerrin's wish was to weight the contributions because k-space center is more important. The assumption is that
	// the k-space center is scanned in the middle of the scan. Therefore, we make a simple ramp-up/down that peaks
	// in the middle of the list of distances.
	count = 0;
	auto weightingFunctionDistances = [&](double current, double val)
	{
		return current + val * (1.0 - 2.0 * std::abs(count++ / (length - 1.0) - 0.5));
	};
	stats_[WeightedMeanDistance] =
		2.0 / (length - 1.0) * std::accumulate(distances.begin(), distances.end(), 0.0, weightingFunctionDistances);

}

std::string XPaceStatistic::getLogFilename()
{
	return file_.getFilename();
}

}