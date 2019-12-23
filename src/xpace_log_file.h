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
    InitialPosition getInitialPosition();
    int getNumberOfMotions();

private:
    std::string fileName_;
    std::vector<Motion> poses_;
    InitialPosition initialPose_;
    bool isXpaceLogFile();
    void parse();
};
}

#endif //XPACETOOLS__XPACE_LOG_FILE_H_
