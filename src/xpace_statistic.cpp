#include "xpace_statistic.h"
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>

xpace::XPaceStatistic::XPaceStatistic(const xpace::XpaceLogFile& file)
        :file_(file), initialPose_(file.getInitialPosition())
{
    calcAbsPositionStatistic();
}

void xpace::XPaceStatistic::calcAbsPositionStatistic()
{
    auto motions = file_.getRelativeMotions();
    auto length = file_.getNumberOfMotions();
    relativePositions_ = std::vector<Vector>();
    for (const auto& m: motions) {
        relativePositions_.push_back(
                m.toAbsoluteCoordinates(initialPose_).t-initialPose_.t
        );
    }

    auto euclideanDistance = [](Vector b) -> double {
      return std::sqrt(b.x*b.x+b.y*b.y+b.z*b.z);
    };

    std::vector<double> distances(length);
    std::transform(relativePositions_.begin(), relativePositions_.end(), distances.begin(), euclideanDistance);
    auto mean = std::accumulate(distances.begin(), distances.end(), 0.0)/length;
    positionStats_[MeanDistance] = mean;

    auto minmax = std::minmax_element(distances.begin(), distances.end());
    positionStats_[MinDistance] = *minmax.first;
    positionStats_[MaxDistance] = *minmax.second;

    auto stdDevFunc = [=](double current, double val) -> double {
      return current+(val-mean)*(val-mean);
    };
    auto stdDev = std::sqrt(std::accumulate(distances.begin(), distances.end(), 0.0, stdDevFunc)/(length-1.0));
    positionStats_[StandardDeviation] = stdDev;
}

