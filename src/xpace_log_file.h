//
// Created by Patrick Scheibe on 18.12.19.
//

#ifndef XPACETOOLS__XPACE_LOG_FILE_H_
#define XPACETOOLS__XPACE_LOG_FILE_H_

#include <string>
#include <regex>

class XpaceLogFile {
public:
    XpaceLogFile(std::string fileName);
private:
    std::string fileName_;
    bool isXpaceLogFile();
    void parse();
};

#endif //XPACETOOLS__XPACE_LOG_FILE_H_
