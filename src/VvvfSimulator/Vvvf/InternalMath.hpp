#ifndef INTERNALMATH_HPP
#define INTERNALMATH_HPP

/*
   Copyright © 2025 VvvfGeeks, VVVF Systems
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// InternalMath.hpp
// Version 1.9.1.1

// Internal Includes
#include "Namespace_VVVF.h"
// Standard Library Includes
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <functional>
#include <span>
#include <type_traits>
// Package Includes
#include <QPointF>

namespace NAMESPACE_VVVF::InternalMath
{
	constexpr double m_2_PI    = M_2_PI;        // 0.63661977236
	constexpr double m_1_PI    = M_1_PI;        // 0.31830988618
	constexpr double m_1_2PI   = 0.15915494309;
	constexpr double m_2PI     = 6.28318530717;
	constexpr double m_PI      = M_PI;          // 3.14159265358
	constexpr double m_PI_2    = M_PI_2;        // 1.57079632679
	constexpr double m_PI_3    = 1.04719755119;
	constexpr double m_PI_4    = 0.78539816339; // M_PI_4
	constexpr double m_PI_6    = 0.52359877559;
	constexpr double m_PI_180  = 0.01745329251;

	constexpr double m_SQRT3   = 1.73205080757;
	constexpr double m_SQRT3_2 = 0.86602540378;

	constexpr double m_E       = M_E;           // 2.71828182846

	namespace Functions
	{
		double saw(double x);
		inline auto sine(auto x)
		{ return std::sin(x); }
		double square(double x);
		inline auto cosine(auto x)
		{ return std::cos(x); }
		double modifiedSine(double x, double level);
		double modifiedSaw(double x);

		template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
		constexpr T iPow(T base, T exp)
		{
			T result = 1;
			if (exp > 0)
			{
				for (T i = 0; i < exp; i++)
					result *= base;
			}
			else if (exp < 0)
			{
				for (T i = 0; i > exp; i--)
					result /= base;
			}
			// else
			return result; // 1
		}

		template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
		constexpr T iLog2(T x)
		{
			T result = 0;
			while (x >>= 1) result++;
			return result;
		}
	
		// Batched functions
		template <typename T>
		QVector<T> sineBatch(const std::span<T> &x);

		template <class ForwardIt, class T>
		inline void iotaVariableStep(ForwardIt first, ForwardIt last, T value, T step)
		{
			while (first != last)
			{
				*(first++) = value;
				value += step;
			}
		}
	}

	namespace EquationSolver
	{
		using Function = std::function<double(double)>;
		enum class EquationSolverType
		{
			Newton, Bissection
		};

		class NewtonMethod
		{
			inline double getDerivative(double x);

			inline double getZeroIntersect(double x);

		public:
			Function function;
			double dx{};
			NewtonMethod(Function fun, double d) : function(fun), dx(d) {}

			double operator()(double begin, double tolerance, unsigned n); 
		};

		struct BisectionMethod
		{
			Function function;
			BisectionMethod(Function fun) : function(fun) {}

			double operator()(double x0, double x1, double tolerance, unsigned N);
		};

		double lagrangePolynomial(double x, const std::span<QPointF>& points);
	}
}

#endif // INTERNALMATH_HPP
