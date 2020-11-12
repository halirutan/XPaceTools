//
// Created by Patrick Scheibe on 09.01.20.
//

#pragma once

#include <vector>
#include <map>
#include <boost/format.hpp>

#include "log_file.hpp"
#include "math.hpp"

namespace xpace
{

class XPaceStatistic
{
public:
	explicit XPaceStatistic(const XpaceLogFile &file);

	enum StatKey
	{
		NetMotion,
		NetMotionEuclideanDistance,
		MeanDistance,
		MinDistance,
		MaxDistance,
		StandardDeviation,
		WeightedMeanDistance,
		IntegratedSpeed,
		PartitionWeightedIntegratedSpeed
	};

	/*
	 * Retrieves one of the statistical measures calculated for the log-file
	 */
	double operator[](StatKey key)
	{
		if (stats_.count(key) != 1) {
			throw std::runtime_error(
				"XPaceStatistic: Value for key is not available or appears several times. This should not happen.");
		}
		return stats_[key];
	}

	[[nodiscard]] const std::vector<double> &getSpeed() const
	{
		return speed_;
	}

	[[nodiscard]] std::vector<std::vector<double>> getTranslations() const
	{
		std::vector<std::vector<double>> result{relativeMotions.size()};
		std::transform(relativeMotions.begin(), relativeMotions.end(), result.begin(),
					   [](const Vector &v)
					   {
						   std::vector<double> res{v.x, v.y, v.z};
						   return res;
					   }
		);
		return result;
	};

	[[nodiscard]] std::vector<std::vector<double>> getRotations() const
	{
		std::vector<std::vector<double>> result{file_.getNumberOfMotions()};
		auto motions = file_.getMotions();
		std::transform(motions.begin(), motions.end(), result.begin(),
					   [](const Motion &m)
					   {
							std::vector<double> res{m.q.qi, m.q.qj, m.q.qk};
							return res;
					   }
		);
		return result;
	}

	std::string getLogFilename();

private:
	XpaceLogFile file_;
	InitialPosition initialPose_;

	// This will be calculated by taking the initial pose and apply a tracked motion to it (this incorporates the
	// rotation *and* the translation. After that, we subtract the initial pose's shift again. Note that this is not
	// equivalent to simply using the shift of the tracked motion!
	std::vector<Vector> relativeMotions;
	Vector meanPosition_;
	std::map<StatKey, double> stats_;
	Motion lastMotion_;
	std::vector<double> rmsMotions_;
	std::vector<double> speed_;

	void calculateStatistics();
};

}