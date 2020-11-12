#pragma once

#include <stdexcept>
#include <vector>
#include <cmath>

namespace xpace
{

/**
 * Recursive implementation of a Gaussian filter that works for 1d signals.
 * Based on "Recursive implementation of the Gaussian filter"
 * by Young, Ian T. van Vliet, Lucas J.
 * DOI: 10.1016/0165-1684(95)00020-E
 */
class GaussianFilter
{
public:
	/**
	 * Construct and initialize Gaussian filter with a specific filter width.
	 * @param sigma Width parameter for the filter
	 */
	explicit GaussianFilter(double sigma);

	/**
	 * Filter a 1d signal with a recursive Gaussian filter.
	 * @param input Input of the gaussian filter
	 */
	std::vector<double> filter(const std::vector<double> &input);

private:
	static constexpr double m_zPoles[] = {
		0.85480,
		1.43749,
		1.61231,
		0.82053,
		1.87415
	};
	constexpr static int filterOrder_ = 5;
	double sigma_;
	double b_[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
	double q_;

	/**
	 * The filter variance is not used directly but transformed into a scaling
	 * parameter q.
	 */
	void calculateScaleQ();

	/**
	 * The coefficients of the recursive scheme depend on the filter scaling parameter
	 * q and the poles.
	 */
	void computeRecursiveCoefficients();

};

}
