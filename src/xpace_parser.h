#ifndef XPACE_PARSER_H
#define XPACE_PARSER_H

#include <string>

/**
 * Below, we kind of repeat data-structures we already have as Vector and Quaternion
 * and yes, this is fucked up, but the spirit parser needs (?) values laid out in a
 * flat structure in order to parse into it.
 */
namespace xpace
{
namespace parser
{


struct initial_pose_t
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double qi = 0.0;
    double qj = 0.0;
    double qk = 0.0;
    double qr = 0.0;
};

struct motion_t
{
    int frame = 0;
    int time = 0;
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double qi = 0.0;
    double qj = 0.0;
    double qk = 0.0;
    double qr = 0.0;
};

bool parseMotion(const std::string &input, motion_t &out);
bool parseInitialPose(const std::string &input, initial_pose_t &out);

}
}

#endif //XPACE_PARSER_H
