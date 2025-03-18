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
#include <type_traits>
// Package Includes
#include <QVector>

namespace NAMESPACE_VVVF::InternalMath
{
	constexpr double m_2_PI    = M_2_PI;   // 0.63661977236
	constexpr double m_1_PI    = M_1_PI;   // 0.31830988618
	constexpr double m_1_2PI   = 0.15915494309;
	constexpr double m_2PI     = 6.28318530717;
	constexpr double m_PI      = M_PI;     // 3.14159265358
	constexpr double m_PI_2    = M_PI_2;   // 1.57079632679
	constexpr double m_PI_3    = 1.04719755119;
	constexpr double m_PI_4    = 0.78539816339;   // M_PI_4
	constexpr double m_PI_6    = 0.52359877559;
	constexpr double m_PI_180  = 0.01745329251;

	constexpr double m_SQRT3   = 1.73205080757;
	constexpr double m_SQRT3_2 = 0.86602540378;

	constexpr double m_E       = M_E;      // 2.71828182846

	class Functions
	{
		static std::function<double(double)> _sine /*= static_cast<double(*)(double)>(std::sin)*/;
	public:
		       static double saw   (double x);
		inline static double sine  (double x)
		{
			return _sine(x);
		}
		       static double square(double x);
		inline static double cosine(double x)
		{
			return getSineFunction() == SineFunctions::StandardLibrary ? std::cos(x) : _sine(x + m_PI_4);
		}
		       static double modifiedSine(double x, double level);
					 static double modifiedSaw(double x);

		enum class SineFunctions
		{
			GetFail          = -1,
			StandardLibrary  =  0,
			InternalFastSine =  1,
			QFastSin         =  2
		};
		
		static bool          setSineFunction(SineFunctions newSetting);
		static SineFunctions getSineFunction();

		template <typename T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
		static constexpr T iPow(T base, T exp)
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
		static constexpr T iLog2(T x)
		{
			T result = 0;
			while (x >>= 1) result++;
			return result;
		}
	};

	namespace EquationSolver
	{
		using Function = std::function<double(double)>;
		enum class EquationSolverType
		{
			Newton, Bissection
		};
		struct Point2d
		{
			double X{}, Y{};
			Point2d(double x = 0.0, double y = 0.0) : X(x), Y(y) {}
			inline bool isZero() const noexcept
			{
				return std::fabs(X) == 0 && std::fabs(Y) == 0;
			}
			constexpr Point2d operator +(const Point2d& b) const noexcept
			{
				const auto a = *this;
                return Point2d(a.X + b.X, a.Y + b.Y);
			}
			constexpr Point2d operator -(const Point2d& b) const noexcept
			{
				const auto a = *this;
                return Point2d(a.X - b.X, a.Y - b.Y);
			}
			constexpr Point2d operator *(const Point2d& b) const noexcept
			{
				const auto a = *this;
                return Point2d(a.X * b.X, a.Y * b.Y);
			}
			constexpr Point2d operator /(const Point2d& b) const
			{
				const auto a = *this;
                return Point2d(a.X / b.X, a.Y / b.Y);
			}
			constexpr static Point2d max(const Point2d& a, const Point2d& b)
			{
				return Point2d(std::max(a.X, b.X), std::max(a.Y, b.Y));
			}
			constexpr static Point2d min(const Point2d& a, const Point2d& b)
			{
				return Point2d(std::min(a.X, b.X), std::min(a.Y, b.Y));
			}
		};

		class NewtonMethod
		{
			double getDerivative(double x)
			{
				double Fxdx = function(x + dx);
				double Fx = function(x);
				double Dy = Fxdx - Fx;
				double Dx = dx;
				double Derivative = Dy / Dx;
				return Derivative;
			}

			double getZeroIntersect(double x)
			{
				return -function(x) / getDerivative(x) + x;
			}
			
		public:
			Function function;
			double dx{};
			constexpr NewtonMethod(Function fun, double d) : function(fun), dx(d) {}

			double calculate(double begin, double tolerance, unsigned n); 
		};
		struct BisectionMethod
		{
			Function function;
			BisectionMethod(Function fun) : function(fun) {}

			double calculate(double x0, double x1, double tolerance, unsigned N);
		};
		double calculateLagrangePolynomial(double x, const QVector<Point2d>& points);
	}
}

#endif // INTERNALMATH_HPP
