//
// Created by Patrick Scheibe on 18.12.19.
//

#include "xpace_log_file.h"

#include <utility>

XpaceLogFile::XpaceLogFile(std::string fileName)
    :fileName_(std::move(fileName))
{
  if (isXpaceLogFile()) {
    parse();
  } else {
    throw std::invalid_argument("File " + fileName_ + " is not a valid XPace logfile.");
  }
}

void XpaceLogFile::parse()
{

}

bool XpaceLogFile::isXpaceLogFile()
{
  return false;
}
