//
// Created by Patrick Scheibe on 09.01.20.
//

#pragma once

#include <vector>
#include <map>
#include <boost/format.hpp>

#include "xpace_log_file.h"
#include "xpace_math.h"

namespace xpace {

class XPaceStatistic {
public:
    explicit XPaceStatistic(const XpaceLogFile& file);

    enum StatKey {
      MeanDistance,
      MinDistance,
      MaxDistance,
      StandardDeviation,
      WeightedMeanDistance
    };

    /*
     * Retrieves one of the statistical measures calculated for the log-file
     */
    double operator[](StatKey key)
    {
        if (positionStats_.count(key)!=1) {
            throw std::runtime_error(
                    "XPaceStatistic: Value for key is not available or appears several times. This should not happen.");
        }
        return positionStats_[key];
    }

    std::string getLogFilename();

private:
    XpaceLogFile file_;
    InitialPosition initialPose_;

    // This will be calculated by taking the initial pose and apply a tracked motion to it (this incorporates the
    // rotation *and* the translation. After that, we subtract the initial pose's shift again. Note that this is not
    // equivalent to simply using the shift of the tracked motion!
    std::vector<Vector> relativePositions_;
    Vector meanPosition_;
    std::map<StatKey, double> positionStats_;

    void calculateStatistics();
};

}