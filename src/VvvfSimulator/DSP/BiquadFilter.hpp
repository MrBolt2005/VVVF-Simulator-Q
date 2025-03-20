#pragma once

// Standard Library
#include <utility>
// Internal
#include "../Vvvf/InternalMath.hpp"
// Packages
#include <Eigen/Dense>

namespace VvvfSimulator::DSP
{
	template <typename T>
	class BiquadFilter
	{
		using VectorX   = Eigen::Matrix<T, Eigen::Dynamic, 1>;
		using ThisClass = BiquadFilter<T>;
		
		VectorX m_a; // Denominator coefficients
		VectorX m_b; // Numerator coefficients
		VectorX m_x; // Input history
		VectorX m_y; // Output history
		
	public:
		BiquadFilter() = delete;
		BiquadFilter(const VectorX& a, const VectorX& b) :
			m_a(a),
			m_b(b),
			m_x(VectorX::Zero(a.size())),
			m_y(VectorX::Zero(a.size()))
		{}
		BiquadFilter(VectorX&& a, VectorX&& b) :
			m_a(a),
			m_b(b),
			m_x(VectorX::Zero(a.size())),
			m_y(VectorX::Zero(a.size()))
		{}
		T process(T input)
		{
			// Shift input history
			m_x.segment(1, m_x.size() - 1) = m_x.head(m_x.size() - 1);
			m_x(0) = input;
	
			// Compute the output
			const T output = m_b.dot(m_x) - m_a.tail(m_a.size() - 1).dot(m_y.head(m_y.size() - 1));
	
			// Shift output history
			m_y.segment(1, m_y.size() - 1) = m_y.head(m_y.size() - 1);
			m_y(0) = output;
	
			return output;
		}
		
		VectorX a() const { return m_a; }
		VectorX b() const { return m_b; }
		constexpr const VectorX& const_a() const noexcept { return m_a; }
		constexpr const VectorX& const_b() const noexcept { return m_b; }

		static std::pair<VectorX, VectorX> calculateLPFCoefficients(T f0, T Q, T Fs)
		{
			using namespace NAMESPACE_VVVF::InternalMath;
			std::pair<VectorX, VectorX> a_b;

			const T w0 = 2 * m_PI * f0 / Fs;
			const T alpha = std::sin(w0) * 0.5 * Q;

			const T cosw0 = cos(w0);
			const T a0 = 1 + alpha;
			const T a1 = -2 * cosw0;
			const T a2 = 1 - alpha;
			const T b0 = (1 - cosw0) / 2;
			const T b1 = 1 - cosw0;
			const T b2 = (1 - cosw0) / 2;

			a_b.first << 1, a1 / a0, a2 / a0;
			a_b.second << b0 / a0, b1 / a0, b2 / a0;

			return a_b;
		}
		static std::pair<VectorX, VectorX> calculateHPFCoefficients(T f0, T Q, T Fs)
		{
			using namespace NAMESPACE_VVVF::InternalMath;
			std::pair<VectorX, VectorX> a_b;

			const T w0 = 2 * m_PI * f0 / Fs;
			const T alpha = std::sin(w0) * 0.5 * Q;

			const T cosw0 = cos(w0);
			const T a0 = 1 + alpha;
			const T a1 = -2 * cosw0;
			const T a2 = 1 - alpha;
			const T b0 = (1 + cosw0) / 2;
			const T b1 = -(1 + cosw0);
			const T b2 = (1 + cosw0) / 2;

			a_b.first << 1, a1 / a0, a2 / a0;
			a_b.second << b0 / a0, b1 / a0, b2 / a0;

			return a_b;
		}
		static std::pair<VectorX, VectorX> calculateBPFCoefficients(T f0, T Q, T Fs)
		{
			using namespace NAMESPACE_VVVF::InternalMath;
			std::pair<VectorX, VectorX> a_b;

			const T w0 = 2 * m_PI * f0 / Fs;
			const T alpha = std::sin(w0) * 0.5 * Q;

			const T cosw0 = cos(w0);
			const T a0 = 1 + alpha;
			const T a1 = -2 * cosw0;
			const T a2 = 1 - alpha;
			const auto &b0 = alpha;
			const T b1 = 0;
			const T b2 = -alpha;

			a_b.first << 1, a1 / a0, a2 / a0;
			a_b.second << b0 / a0, b1 / a0, b2 / a0;

			return a_b;
		}
		static std::pair<VectorX, VectorX> calculateNotchCoefficients(T f0, T Q, T Fs)
		{
			using namespace NAMESPACE_VVVF::InternalMath;
			std::pair<VectorX, VectorX> a_b;

			const T w0 = 2 * m_PI * f0 / Fs;
			const T alpha = std::sin(w0) * 0.5 * Q;
			const T cosw0 = cos(w0);

			const T a0 = 1 + alpha;
			const T a1 = -2 * cosw0;
			const T a2 = 1 - alpha;
			const T b0 = 1;
			const T b1 = -2 * cosw0;
			const T b2 = 1;

			a_b.first << 1, a1 / a0, a2 / a0;
			a_b.second << b0 / a0, b1 / a0, b2 / a0;

			return a_b;
		}
		static std::pair<VectorX, VectorX> calculateAllPassCoefficients(T f0, T Q, T Fs)
		{
			using namespace NAMESPACE_VVVF::InternalMath;
			std::pair<VectorX, VectorX> a_b;

			const T w0 = 2 * m_PI * f0 / Fs;
			const T alpha = std::sin(w0) * 0.5 * Q;
			const T cosw0 = cos(w0);

			const T a0 = 1 + alpha;
			const T a1 = -2 * cosw0;
			const T a2 = 1 - alpha;
			const T b0 = 1 - alpha;
			const T b1 = -2 * cosw0;
			const T b2 = 1 + alpha;

			a_b.first << 1, a1 / a0, a2 / a0;
			a_b.second << b0 / a0, b1 / a0, b2 / a0;

			return a_b;
		}
		static std::pair<VectorX, VectorX> calculatePeakingEQCoefficients(T f0, T Q, T gain, T Fs, bool is_gain_in_dB = false)
		{
			using namespace NAMESPACE_VVVF::InternalMath;
			std::pair<VectorX, VectorX> a_b;

			if (is_gain_in_dB)
				gain = std::pow(10, gain / 20);
			const T w0 = 2 * m_PI * f0 / Fs;
			const T alpha = std::sin(w0) * 0.5 * Q;
			const T cosw0 = cos(w0);

			const T a0 = 1 + alpha / gain;
			const T a1 = -2 * cosw0;
			const T a2 = 1 - alpha / gain;
			const T b0 = 1 + alpha * gain;
			const T b1 = -2 * cosw0;
			const T b2 = 1 - alpha * gain;

			a_b.first << 1, a1 / a0, a2 / a0;
			a_b.second << b0 / a0, b1 / a0, b2 / a0;

			return a_b;
		}
	};
}

//#include "BiquadFilter.tpp"