//
// Created by Patrick Scheibe on 18.12.19.
//

#include "xpace_log_file.h"

#include <stdexcept>
#include <fstream>
#include <regex>
#include <cstdio>
#include <boost/spirit/home/x3.hpp>

XpaceLogFile::XpaceLogFile(std::string fileName)
    : fileName_(std::move(fileName))
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
void XpaceLogFile::parse()
{
    // we already tested if the file is valid and can be opened!
    std::ifstream file;
    file.open(fileName_);
    if (file.is_open()) {
        std::string line;
        char buffer[2018];
        while (std::getline(file, line)) {
            double x, y, z, qr, qi, qj, qk;
            if (line.length() > 0) {
                char &firstChar = line[0];
                int itemsParsed;
                switch (firstChar) {
                    case '@':
                        itemsParsed = sscanf(buffer, "@%f %f %f ( %f %f %f ) %f",
                            &x, &y, &z, &qi, &qj, &qk, &qr
                            );
                        //TODO: Ok, late enough.. go on here.
                }

                /*
                struct trans_t p;
                int numItems;
                switch (buffer[0]) {
                // Initial Pose starts with a @ in the logfile
                case '@':
                    numItems = sscanf(buffer, "@%f %f %f ( %f %f %f ) %f",
                        &(p.x), &(p.y), &(p.z), &(p.qi), &(p.qj), &(p.qk), &(p.qr));
                    // Check if we got the right number of values
                    if(numItems != 7) {
                        return false;
                    }
                    p.time = 0;
                    p.frame = 0;
                    mInitialPose = p;
                    break;
                // Comment
                case '#':
                    break;
                // Some error thingie we're going to ignore
                case '[':
                    break;
                // All other lines in the logfile should be motion measures
                // They start with a number, but we don't check for that.
                // We ruthlessly try to match the values.
                default:
                    numItems = sscanf(buffer, "%d %d %f %f %f ( %f %f %f ) %f",
                        &(p.frame), &(p.time), &(p.x), &(p.y), &(p.z), &(p.qi),
                        &(p.qj), &(p.qk), &(p.qr));
                    // Check if we got the right number of values
                    if(numItems != 9) {
                        return false;
                    }
                    mData.push_back(p);
                    break;
                }

            }
        }
        return true;
    }
    return false;
}
                 */
            }
        }
    } else {
        throw std::runtime_error("Logfile cannot be opened. That should never happen.");
    }

}

/**
 * Checks if the filename is *.log and if we have the xpace magic string in the file
 * @return true if it appears to be a valid logfile
 */
bool XpaceLogFile::isXpaceLogFile()
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

// see https://www.boost.org/doc/libs/1_71_0/libs/spirit/doc/x3/html/spirit_x3/quick_reference/string.html
void parseContent(){
    using namespace boost::spirit::x3;
    auto magicRule = "# libXPACE log file" >> eol;
    auto initialPoseRule = char_('@') >> double_ >> double_ >> double_ >> char_('(') >> double_ >> double_ >> double_ >> char_(')') >> double_ >> eol;
    auto motionRule = int_ >> int_ >> double_ >> double_ >> double_ >> char_('(') >> double_ >> double_ >> double_ >> char_(')') >> double_;
}
