//
// Created by Patrick Scheibe on 18.12.19.
//

#include "xpace_log_file.h"
#include "xpace_parser.h"

#include <stdexcept>
#include <fstream>
#include <regex>
#include <iostream>

xpace::XpaceLogFile::XpaceLogFile(std::string fileName)
    : fileName_(std::move(fileName)), initialPose_(), poses_()
{
    if (isXpaceLogFile()) {
        parse();
    }
    else {
        throw std::invalid_argument("File " + fileName_ + " is not a valid XPace logfile.");
    }
}

/**
 * Parses the logfile. For internal use only.
 */
void xpace::XpaceLogFile::parse()
{
    // we already tested if the file is valid and can be opened!
    std::ifstream file;
    file.open(fileName_);
    bool haveInitialPose = false;
    if (file.is_open()) {
        std::string line;
        while(std::getline(file, line)) {
            if (!haveInitialPose) {
                xpace::parser::initial_pose_t pose;
                bool result = xpace::parser::parseInitialPose(line, pose);
                if (result) {
                    haveInitialPose = true;
                    initialPose_ = pose;
                }
                continue;
            }

            xpace::parser::motion_t motion;
            bool result = xpace::parser::parseMotion(line, motion);
            if (result) {
                poses_.emplace_back(motion);
            }

        }

    }
    else {
        throw std::runtime_error("Logfile cannot be opened. That should never happen.");
    }

    if (!haveInitialPose || poses_.empty()) {
        throw std::runtime_error("Could not create log-file object. Either no initial pose or no motions could be read.");
    }

}

/**
 * Checks if the filename is *.log and if we have the xpace magic string in the file
 * @return true if it appears to be a valid logfile
 */
bool xpace::XpaceLogFile::isXpaceLogFile()
{
    if (std::regex_match(fileName_, std::regex(".*.log"))) {
        std::ifstream file;
        file.open(fileName_, std::ios::in);
        if (file.is_open()) {
            std::string firstLine;
            std::getline(file, firstLine);
            if (std::regex_match(firstLine, std::regex("# libXPACE log file[\r\n]*"))) {
                file.close();
                return true;
            }
        }
        file.close();
    }
    return false;
}

xpace::InitialPosition xpace::XpaceLogFile::getInitialPosition()
{
    return initialPose_;
}

int xpace::XpaceLogFile::getNumberOfMotions()
{
    return poses_.size();
}
