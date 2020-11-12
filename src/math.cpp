#include "math.hpp"
#include <iostream>
#include <cmath>

namespace xpace
{

InitialPosition::InitialPosition()
	: q(), t()
{

}

InitialPosition::InitialPosition(const parser::initial_pose_t &initialPose)
{
	t = {initialPose.x, initialPose.y, initialPose.z,};
	q = {initialPose.qr, initialPose.qi, initialPose.qj, initialPose.qk};
}

InitialPosition &InitialPosition::operator=(const parser::initial_pose_t &initialPose)
{
	t = {initialPose.x, initialPose.y, initialPose.z,};
	q = {initialPose.qr, initialPose.qi, initialPose.qj, initialPose.qk};
	return *this;
}

InitialPosition::InitialPosition(const InitialPosition &other) = default;

std::ostream &operator<<(std::ostream &os, const InitialPosition &p)
{
	auto prec = os.precision();
	os.precision(10);
	os << "Initial Pose trans(" << p.t.x << ", " << p.t.y << ", " << p.t.z << ") quat("
	   << p.q.qr << ", " << p.q.qi << ", " << p.q.qj << ", " << p.q.qk << ")";
	os.precision(prec);
	return os;
}

Motion::Motion()
	: q(), t(), time(), frame()
{

}

Motion::Motion(const parser::motion_t &motion)
{
	time = motion.time;
	frame = motion.frame;
	t = {motion.x, motion.y, motion.z};
	q = {motion.qr, motion.qi, motion.qj, motion.qk};
}

Motion Motion::applyToPose(const InitialPosition &initialPose) const
{
	auto t0 = initialPose.t;
	auto q0 = initialPose.q;
	auto result = Motion(*this);
	result.t = rotate(t0, q) + t;
	result.q = q * q0;
	return result;
}

std::ostream &operator<<(std::ostream &os, const Motion &m)
{
	os.precision(10);
	os << "Time: " << m.time << " Frame: " << m.frame << " trans(" << m.t.x << ", " << m.t.y << ", " << m.t.z
	   << ") quat("
	   << m.q.qr << ", " << m.q.qi << ", " << m.q.qj << ", " << m.q.qk << ")";
	return os;
}

double Motion::euclideanDistance() const
{
	return std::sqrt(t.x*t.x + t.y*t.y + t.z*t.z + q.qi*q.qi + q.qj*q.qj + q.qk*q.qk);
}

Vector rotate(const Vector &v, const Quaternion &q)
{
	Quaternion qv{0.0, v.x, v.y, v.z};
	Quaternion r = q * qv * q.conjugate();
	return {r.qi, r.qj, r.qk};
}

}