//
// Created by Patrick Scheibe on 09.01.20.
//

#ifndef XPACETOOLS_XPACE_STATISTIC_H
#define XPACETOOLS_XPACE_STATISTIC_H

#include <vector>
#include "xpace_log_file.h"
#include "xpace_math.h"

namespace xpace {

class XPaceStatistic {
public:
    explicit XPaceStatistic(const XpaceLogFile& motions);

private:
    InitialPosition initialPose_;
    std::vector<Vector> absPositions_;
    Vector meanPosition_;

    void calcAbsPositionStatistic();
};

} // end namespace xpace

#endif //XPACETOOLS_XPACE_STATISTIC_H
