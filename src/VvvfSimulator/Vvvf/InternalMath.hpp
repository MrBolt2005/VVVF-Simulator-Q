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
// Package Includes
#include <QVector>

namespace NAMESPACE_VVVF::InternalMath
{
	constexpr double M_2_PI    = std::M_2_PI;   // 0.63661977236
	constexpr double M_1_PI    = std::M_1_PI;   // 0.31830988618
	constexpr double M_1_2PI   = 0.15915494309;
	constexpr double M_2PI     = 6.28318530717;
	constexpr double M_PI      = std::M_PI;     // 3.14159265358
	constexpr double M_PI_2    = std::M_PI_2;   // 1.57079632679
	constexpr double M_PI_3    = 1.04719755119;
	constexpr double M_PI_4    = std::M_PI_4;   // 0.78539816339
	constexpr double M_PI_6    = 0.52359877559;
	constexpr double M_PI_180  = 0.01745329251;

	constexpr double M_SQRT3   = 1.73205080757;
	constexpr double M_SQRT3_2 = 0.86602540378;

	class Functions
	{
		static std::function<double(double)> _sine = std::sin;
	public:
		       static double saw(double x);
		inline static double sine(double x)
		{
			return _sine(x);
		}
		       static double square(double x);
		inline static double cosine(double x)
		{
			return getSineFunction() == SineFunctions::StandardLibrary ? std::cos(x) : _sine(x + M_PI_4);
		}

		enum class SineFunctions:uint_fast8_t
		{
			StandardLibrary, InternalFastSine, QtFastSine
		}
		
		static bool          setSineFunction(SineFunctions newSetting);
		static SineFunctions getSineFunction();
	}

	namespace EquationSolver
	{
		using std::function<double(double)> Function;
		enum class EquationSolverType
		{
			Newton, Bissection
		};
		class Point2d
		{
		public:
			       double X{}, Y{};
			inline bool isZero() const
			{
				return std::fabs(X) == 0 && std::fabs(Y) == 0;
			}
			inline static Point2d operator +(const Point2d& a, const Point2d& b)
			{
				return Point2d(a.X + b.X, a.Y + b.Y);
			}
			inline static Point2d operator -(const Point2d& a, const Point2d& b)
			{
				return Point2d(a.X - b.X, a.Y - b.Y);
			}
			inline static Point2d operator *(const Point2d& a, const Point2d& b)
			{
				return Point2d(a.X * b.X, a.Y * b.Y);
			}
			inline static Point2d operator /(const Point2d& a, const Point2d& b)
			{
				return Point2d(a.X / b.X, a.Y / b.Y);
			}
			inline static Point2d max(const Point2d& a, const Point2d& b)
			{
				return Point2d(std::max(a.X, b.X), std::max(a.Y, b.Y));
			}
			inline static Point2d min(const Point2d& a, const Point2d& b)
			{
				return Point2d(std::min(a.X, b.X), std::min(a.Y, b.Y));
			}
		}

		calculateNewtonMethod   (Function function, double dx, double begin, double tolerance, size_t n);
		calculateBisectionMethod(Function function, double begin, double tolerance, size_t N);
		calculateLagrangePolynomial(double x, QVector<Point2d> points);
	}
}

#endif // INTERNALMATH_HPP
