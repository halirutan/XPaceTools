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
	calculateStatistics();
}

void XPaceStatistic::calculateStatistics()
{
	auto motions = file_.getRelativeMotions();
	auto length = file_.getNumberOfMotions();

	// Each tracked motion is transformed into scanner coordinates which incorporates both the shift t and the
	// rotational part q. Then we subtract the initial position from the resulting point to get the vector of how
	// much the tracked motion moves the mouthpiece from its initial position.
	// This appears to me to be a more useful insight than looking purely at the motions.
	relativePositions_ = std::vector<Vector>();
	for (const auto &m: motions) {
		relativePositions_.push_back(
			m.toAbsoluteCoordinates(initialPose_).t - initialPose_.t
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
	positionStats_[MeanDistance] = mean;

	auto minmax = std::minmax_element(distances.begin(), distances.end());
	positionStats_[MinDistance] = *minmax.first;
	positionStats_[MaxDistance] = *minmax.second;

	auto stdDevFunc = [=](double current, double val) -> double
	{
		return current + (val - mean) * (val - mean);
	};
	auto stdDev = std::sqrt(std::accumulate(distances.begin(), distances.end(), 0.0, stdDevFunc) / (length - 1.0));
	positionStats_[StandardDeviation] = stdDev;

	// Kerrin's wish was to weight the contributions because k-space center is more important. The assumption is that
	// the k-space center is scanned in the middle of the scan. Therefore, we make a simple ramp-up/down that peaks
	// in the middle of the list of distances.
	auto count = 0;
	auto weightedEuclideanDistance = [&](double current, double val)
	{
		return current + val * (1.0 - 2.0 * std::abs(count++ / (length - 1.0) - 0.5));
	};
	positionStats_[WeightedMeanDistance] =
		2.0 / (length - 1.0) * std::accumulate(distances.begin(), distances.end(), 0.0, weightedEuclideanDistance);

}

std::string XPaceStatistic::getLogFilename()
{
	return file_.getFilename();
}

}