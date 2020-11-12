#pragma once
#include <string>

/**
 * Below, we kind of repeat data-structures we already have as Vector and Quaternion
 * and yes, this is fucked up, but the spirit parser needs (?) values laid out in a
 * flat structure in order to parse into it.
 */
namespace xpace::parser {

/**
 * Internal struct that the parser can directly assign when reading the initial pose in the logfile
 */
struct initial_pose_t {
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double qi = 0.0;
  double qj = 0.0;
  double qk = 0.0;
  double qr = 0.0;
};

/**
 * Internal struct that the parser can directly assign when reading a motion line
 */
struct motion_t {
  unsigned int frame = 0;
  unsigned int time = 0;
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;
  double qi = 0.0;
  double qj = 0.0;
  double qk = 0.0;
  double qr = 0.0;
};

/**
 * Takes one input line and tries to parse a motion from it.
 * @param input Input line from the log file
 * @param out Motion that is filled when the parse is successful
 * @return True if the line could be parsed as a motion
 */
bool parseMotion(const std::string& input, motion_t& out);

/**
 * Takes one input line and tries to parse an initial pose from it.
 * @param input Input line from the log file
 * @param out Initial pose that is filled when the parse is successful
 * @return True if the line could be parsed as an initial pose
 */
bool parseInitialPose(const std::string& input, initial_pose_t& out);

}