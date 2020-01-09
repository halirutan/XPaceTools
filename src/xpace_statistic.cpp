//
// Created by Patrick Scheibe on 09.01.20.
//

#include "xpace_statistic.h"
#include <vector>
#include <numeric>
#include <cmath>

xpace::XPaceStatistic::XPaceStatistic(const xpace::XpaceLogFile& logFile)
        :initialPose_(logFile.getInitialPosition())
{
    auto motions = logFile.getRelativeMotions();
    auto length = logFile.getNumberOfMotions();
    absPositions_ = std::vector<Vector>(length);
    for (const auto& m: motions) {
        absPositions_.push_back(
                m.toAbsoluteCoordinates(initialPose_).t-initialPose_.t
        );
    }
    const Vector& total = std::accumulate(absPositions_.begin(), absPositions_.end(), Vector{0.0, 0.0, 0.0});
    meanPosition_ = Vector{total.x/length, total.y/length, total.z/length};

    auto distanceAcc = [](double a, Vector b) -> double {
      return a+std::sqrt(b.x*b.x+b.y*b.y+b.z*b.z);
    };
    std::accumulate(absPositions_.begin(), absPositions_.end(), 0.0,
            distanceAcc
    );
}

void xpace::XPaceStatistic::calcAbsPositionStatistic()
{

}

