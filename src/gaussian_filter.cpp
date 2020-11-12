#include "gaussian_filter.hpp"

#include <iostream>

namespace xpace
{

GaussianFilter::GaussianFilter(double sigma)
	: sigma_(sigma), q_()
{
	if (sigma < 0.5 || sigma > 1000)
		throw std::domain_error("Parameter value for sigma out of range [0.5,1000.0]");

	calculateScaleQ();
	computeRecursiveCoefficients();
}

std::vector<double> GaussianFilter::filter(const std::vector<double> &input)
{
	auto m_b1 = b_[0], m_b2 = b_[1], m_b3 = b_[2], m_b4 = b_[3], m_b5 = b_[4];
	auto pad = std::max(30, static_cast<int>(4 * sigma_) + filterOrder_);
	auto alpha = 1.0 + m_b1 + m_b2 + m_b3 + m_b4 + m_b5;
	auto paddedWidth = static_cast<int>(input.size() + pad * 2);

	auto v = std::vector<double>(paddedWidth, 0.0);
	std::copy(input.begin(), input.end(), v.begin() + pad);

	for (int i = pad; i != paddedWidth; ++i) {
		v[i] = alpha * v[i]
			- (m_b1 * v[i - 1] + m_b2 * v[i - 2] + m_b3 * v[i - 3] + m_b4 * v[i - 4] + m_b5 * v[i - 5]);
	}
	for (int i = paddedWidth - 1 - filterOrder_; i != 0; --i) {
		v[i] = alpha * v[i]
			- (m_b1 * v[i + 1] + m_b2 * v[i + 2] + m_b3 * v[i + 3] + m_b4 * v[i + 4] + m_b5 * v[i + 5]);
	}

	std::vector<double> result(input.size());
	std::copy(v.begin() + pad, v.end() - pad, result.begin());
	return result;
}

void GaussianFilter::calculateScaleQ()
{
	double const
		m = 2.2001668904030254,
		n = 0.0004933351422627583,
		h = 0.8458497161901605;
	double const msq = m * m;
	q_ = ((sqrt(h * msq + msq * sigma_ * sigma_) - m * n) / msq);
}

void GaussianFilter::computeRecursiveCoefficients()
{
	double d1r = m_zPoles[0],
		d1i = m_zPoles[1],
		d3r = m_zPoles[2],
		d3i = m_zPoles[3],
		d5r = m_zPoles[4];
	double qinv = 1.0 / q_;
	double
		argd1 = atan2(d1i, d1r),
		argd3 = atan2(d3i, d3r),
		sabsd1 = d1r * d1r + d1i * d1i,
		sabsd3 = d3r * d3r + d3i * d3i;
	double
		cosd1 = cos(argd1 * qinv),
		cosd3 = cos(argd3 * qinv);
	double
		d1pow1 = pow(sabsd1, qinv),
		d3pow1 = pow(sabsd3, qinv),
		d5rpow1 = pow(d5r, qinv),
		d1pow1i = pow(sabsd1, -qinv),
		d3pow1i = pow(sabsd3, -qinv),
		d5rpow1i = pow(d5r, -qinv),
		d1pow2 = pow(sabsd1, 0.5 * qinv),
		d3pow2 = pow(sabsd3, 0.5 * qinv),
		d1pow2i = pow(sabsd1, -0.5 * qinv),
		d3pow2i = pow(sabsd3, -0.5 * qinv);

	b_[0] = -d5rpow1i - 2.0 * d1pow2i * cosd1 - 2.0 * d3pow2i * cosd3;
	b_[1] = d1pow1i + d3pow1i + 2.0 * d3pow2i * d5rpow1i * cosd3
		+ cosd1 * (2.0 * d1pow2i * d5rpow1i + 4.0 * d1pow2i * d3pow2i * cosd3);
	b_[2] = d1pow1i * d3pow1i * d5rpow1i * (-d1pow1 - d3pow1 - 2.0 * d3pow2 * d5rpow1 * cosd3
		- 2.0 * cosd1 * (d1pow2 * d5rpow1 + 2.0 * d1pow2 * d3pow2 * cosd3));
	b_[3] = d1pow1i * d3pow1i * d5rpow1i * (d5rpow1 + 2.0 * d1pow2 * cosd1 +
		2.0 * d3pow2 * cosd3);
	b_[4] = -d1pow1i * d3pow1i * d5rpow1i;
}
}