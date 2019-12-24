//
// Created by Patrick Scheibe on 18.12.19.
//

#ifndef XPACETOOLS__XPACE_LOG_FILE_H_
#define XPACETOOLS__XPACE_LOG_FILE_H_

#include "xpace_math.h"

#include <string>
#include <regex>


namespace xpace
{

class XpaceLogFile
{
public:
    explicit XpaceLogFile(std::string fileName);
    InitialPosition getInitialPosition() const;

    /**
     * Returns the number of subject motions that were tracked by
     * the PMC system. For a normal scan this can be > 50k entries.
     * @return Number of tracked motions
     */
    size_t getNumberOfMotions() const;

    /**
     * Returns the absolute position of the mouth-piece in scanner
     * coordinates for each tracked motion of the PMC system.
     * @return Absolute positions of tracked motions
     */
    std::vector<Motion> getAbsolutePositions() const;

    /**
     * Returns the relative motions as they were tracked by the PMC
     * system. Note that these values represent the deviations of the
     * mouth-piece from its initial position.
     * @see getInitialPosition()
     * @return Tracked relative motions
     */
    std::vector<Motion> getRelativeMotions() const;


private:
    std::string fileName_;
    std::vector<Motion> motions_;
    std::vector<Motion> positions_;
    InitialPosition initialPose_;
    bool isXpaceLogFile();
    void parse();
    void calculateAbsolutePositions();
};

} // namespace xparse

#endif //XPACETOOLS__XPACE_LOG_FILE_H_
