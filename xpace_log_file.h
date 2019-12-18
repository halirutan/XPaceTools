//
// Created by Patrick Scheibe on 18.12.19.
//

#ifndef XPACETOOLS__XPACE_LOG_FILE_H_
#define XPACETOOLS__XPACE_LOG_FILE_H_

#include <string>

class XpaceLogFile {
public:
    XpaceLogFile(std::string fileName);
private:
    std::string fileName_;
    static bool isXpaceLogFile();
    static void parse();
};

#endif //XPACETOOLS__XPACE_LOG_FILE_H_
