//
// Created by Patrick Scheibe on 09.01.20.
//

#pragma once

#include <vector>
#include <map>
#include <boost/format.hpp>

#include "log_file.hpp"
#include "math.hpp"
#include "gaussian_filter.hpp"

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

	[[nodiscard]] std::vector<double> getSpeed(double filterSigma) const
	{
		if (filterSigma < 1.0 || filterSigma > 1000.0) {
			return speed_;
		}
		GaussianFilter filter(filterSigma);
		auto result = filter.filter(speed_);
		return result;
	}

	[[nodiscard]] std::vector<std::vector<double>> getTranslations() const
	{
		std::vector<std::vector<double>> result{positions_.size()};
		std::transform(positions_.begin(), positions_.end(), result.begin(),
					   [](const Vector &v)
					   {
						   std::vector<double> res{v.x, v.y, v.z};
						   return res;
					   }
		);
		return result;
	};

	[[nodiscard]] std::vector<double> getTranslationXFiltered(double filterSigma) const
	{
		std::vector<double> result(positions_.size());
		std::transform(positions_.begin(), positions_.end(), result.begin(),
					   [](const Vector &v)
					   {
						   return v.x;
					   }
		);
		if (filterSigma < 1.0 || filterSigma > 1000.0) {
			return result;
		}
		GaussianFilter gauss(filterSigma);
		return gauss.filter(result);
	};

	[[nodiscard]] std::vector<double> getTranslationYFiltered(double filterSigma) const
	{
		std::vector<double> result(positions_.size());
		std::transform(positions_.begin(), positions_.end(), result.begin(),
					   [](const Vector &v)
					   {
						   return v.y;
					   }
		);
		if (filterSigma < 1.0 || filterSigma > 1000.0) {
			return result;
		}
		GaussianFilter gauss(filterSigma);
		return gauss.filter(result);
	};

	[[nodiscard]] std::vector<double> getTranslationZFiltered(double filterSigma) const
	{
		std::vector<double> result(positions_.size());
		std::transform(positions_.begin(), positions_.end(), result.begin(),
					   [](const Vector &v)
					   {
						   return v.z;
					   }
		);
		if (filterSigma < 1.0 || filterSigma > 1000.0) {
			return result;
		}
		GaussianFilter gauss(filterSigma);
		return gauss.filter(result);
	};

	[[nodiscard]] std::vector<std::vector<double>> getRotations() const
	{
		std::vector<std::vector<double>> result{angles_.size()};
		std::transform(angles_.begin(), angles_.end(), result.begin(),
					   [](const EulerAngle &m)
					   {
							std::vector<double> res{m.roll, m.pitch, m.yaw};
							return res;
					   }
		);
		return result;
	}

	[[nodiscard]] std::vector<double> getRotationRollFiltered(double filterSigma) const
	{
		std::vector<double> result(angles_.size());
		std::transform(angles_.begin(), angles_.end(), result.begin(),
					   [](const EulerAngle &angle)
					   {
						   return angle.roll;
					   }
		);
		if (filterSigma < 1.0 || filterSigma > 1000.0) {
			return result;
		}
		GaussianFilter gauss(filterSigma);
		return gauss.filter(result);
	};

	[[nodiscard]] std::vector<double> getRotationPitchFiltered(double filterSigma) const
	{
		std::vector<double> result(angles_.size());
		std::transform(angles_.begin(), angles_.end(), result.begin(),
					   [](const EulerAngle &angle)
					   {
						   return angle.pitch;
					   }
		);
		if (filterSigma < 1.0 || filterSigma > 1000.0) {
			return result;
		}
		GaussianFilter gauss(filterSigma);
		return gauss.filter(result);
	};


	[[nodiscard]] std::vector<double> getRotationYawFiltered(double filterSigma) const
	{
		std::vector<double> result(angles_.size());
		std::transform(angles_.begin(), angles_.end(), result.begin(),
					   [](const EulerAngle &angle)
					   {
						   return angle.yaw;
					   }
		);
		if (filterSigma < 1.0 || filterSigma > 1000.0) {
			return result;
		}
		GaussianFilter gauss(filterSigma);
		return gauss.filter(result);
	};


	[[nodiscard]] std::vector<double> getTimes() const
	{
		return times_;
	}

	std::string getLogFilename();

private:
	XpaceLogFile file_;
	InitialPosition initialPose_;
	std::vector<double> times_;
	std::vector<Vector> positions_;
	std::vector<EulerAngle> angles_;
	std::vector<double> distances_;
	std::map<StatKey, double> stats_;
	Motion lastMotion_;
	std::vector<double> speed_;
	void calculateStatistics();
};

}