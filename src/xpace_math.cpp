#include "xpace_math.h"
#include <iostream>

xpace::InitialPosition::InitialPosition()
        :q(), t()
{

}

xpace::InitialPosition::InitialPosition(const xpace::parser::initial_pose_t& initialPose)
{
    t = {initialPose.x, initialPose.y, initialPose.z,};
    q = {initialPose.qr, initialPose.qi, initialPose.qj, initialPose.qk};
}

xpace::InitialPosition& xpace::InitialPosition::operator=(const xpace::parser::initial_pose_t& initialPose)
{
	t = {initialPose.x, initialPose.y, initialPose.z,};
	q = {initialPose.qr, initialPose.qi, initialPose.qj, initialPose.qk};
	return *this;
}

xpace::InitialPosition::InitialPosition(const xpace::InitialPosition &other) = default;

std::ostream& xpace::operator<<(std::ostream& os, const xpace::InitialPosition& p)
{
    os.precision(10);
    os << "Initial Pose trans(" << p.t.x << ", " << p.t.y << ", " << p.t.z << ") quat("
       << p.q.qr << ", " << p.q.qi << ", " << p.q.qj << ", " << p.q.qk << ")";
    return os;
}

xpace::Motion::Motion()
        :q(), t(), time(), frame()
{

}

xpace::Motion::Motion(const xpace::parser::motion_t& motion)
{
    time = motion.time;
    frame = motion.frame;
    t = {motion.x, motion.y, motion.z};
    q = {motion.qr, motion.qi, motion.qj, motion.qk};
}

xpace::Motion xpace::Motion::toAbsoluteCoordinates(const xpace::InitialPosition& initialPosition) const
{
    auto t0 = initialPosition.t;
    auto q0 = initialPosition.q;
    auto result = Motion(*this);
    result.t = xpace::rotate(t0, q)+t;
    result.q = q*q0;
    return result;
}

std::ostream& xpace::operator<<(std::ostream& os, const xpace::Motion& m)
{
    os.precision(10);
    os << "Time: " << m.time << " Frame: " << m.frame << " trans(" << m.t.x << ", " << m.t.y << ", " << m.t.z
       << ") quat("
       << m.q.qr << ", " << m.q.qi << ", " << m.q.qj << ", " << m.q.qk << ")";
    return os;
}

xpace::Vector xpace::rotate(const xpace::Vector& v, const xpace::Quaternion& q)
{
    xpace::Quaternion qv{0.0, v.x, v.y, v.z};
    xpace::Quaternion r = q*qv*q.conjugate();
    return {r.qi, r.qj, r.qk};
}
