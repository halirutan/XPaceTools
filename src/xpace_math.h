#ifndef XPACETOOLS_MATH_H
#define XPACETOOLS_MATH_H

#include <cmath>
#include <initializer_list>
#include "xpace_parser.h"

namespace xpace {

class Quaternion {
public:
    double qr;
    double qi;
    double qj;
    double qk;

    Quaternion()
            :qr(0.0), qi(0.0), qj(0.0), qk(0.0) { };

    Quaternion(double qr_, double qi_, double qj_, double qk_)
            :qr(qr_), qi(qi_), qj(qj_), qk(qk_) { };

    Quaternion(const Quaternion& q) = default;

    Quaternion& operator=(const Quaternion& q) = default;

    /**
     * @brief Quaternion multiplication from the right.
     * @param q Right operand of the multiplication
     * @return Result of the multiplication
     *
     * Quaternion multiplication is not commutative, i.e. in general, q1*q2
     * is NOT equal to q2*q1. This function multiplies q from the right to the
     * instance.
     */
    Quaternion operator*(const Quaternion& q) const
    {
        return {
                -qi*q.qi-qj*q.qj-qk*q.qk+q.qr*qr,
                -qk*q.qj+qj*q.qk+qi*q.qr+q.qi*qr,
                qk*q.qi-qi*q.qk+qj*q.qr+q.qj*qr,
                -qj*q.qi+qi*q.qj+qk*q.qr+q.qk*qr
        };
    }

    Quaternion conjugate() const
    {
        return {qr, -qi, -qj, -qk};
    }

    bool isUnitQuaternion()
    {
        return fabs(norm()-1.0)<eps;
    }

    double norm()
    {
        return qr*qr+qi*qi+qj*qj+qk*qk;
    }

private:
    static constexpr double eps = 1.0e-5;
};

class Vector {
public:
    double x;
    double y;
    double z;

    Vector()
            :x(0.0), y(0.0), z(0.0) { };

    Vector(double x_, double y_, double z_)
            :x(x_), y(y_), z(z_) { };

    Vector(const Vector& v) = default;

    Vector& operator=(const Vector& v) = default;

    Vector operator+(const Vector& v)
    {
        return {x+v.x, y+v.y, z+v.z};
    }

    Vector operator-(const Vector& v)
    {
        return {x-v.x, y-v.y, z-v.z};
    }

    Vector operator-()
    {
        return {-x, -y, -z};
    }

    Vector abs()
    {
        return {std::abs(x), std::abs(y), std::abs(z)};
    }

};

/**
 * An "initial pose" consists of a translational part and a rotational part (represented as a quaternion).
 * Based on this initial pose, all Motion records, which are relative movements, can be converted to absolute
 * scanner coordinate.
 */
class InitialPosition {
public:
    Vector t;
    Quaternion q;
    InitialPosition();
    InitialPosition(const parser::initial_pose_t& pose);
};

std::ostream& operator<<(std::ostream& os, const InitialPosition& p);

/**
 * A Motion is a relative motion of the subject with regards to the initial position during the scan. Using the
 * initial position, an absolute position of each motion can be calculated.
 */
class Motion {
public:
    Vector t;
    Quaternion q;
    int time;
    int frame;
    Motion();
    explicit Motion(const parser::motion_t& motion);
    Motion(const Motion&) = default;

    /**
     * Uses an initial position as returned from the log file and converts this motion
     * into a absolute scanner coordinates.
     * @param initialPose
     * @return
     */
    Motion toAbsoluteCoordinates(const InitialPosition& initialPose) const;
};

std::ostream& operator<<(std::ostream& os, const Motion& p);

/**
 * Rotates a vector v about the quaternion q
 * @param v Vector to be rotated
 * @param q Quaternion defining the rotation
 * @return Rotated vector v
 */
Vector rotate(const Vector& v, const Quaternion& q);

} // namespace xpace



#endif //XPACETOOLS_MATH_H
