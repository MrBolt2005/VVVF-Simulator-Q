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

// InternalMath.cpp
// Version 1.9.1.1

// Internal
#include "InternalMath.hpp" // To specialize
// Packages
#include <QtMath>

namespace NAMESPACE_VVVF::InternalMath
{
	double Functions::triangle(double x)
	{
		double fixed_x = x - std::floor(x * m_1_2PI) * m_2PI;
		if (0 <= fixed_x && fixed_x < m_PI_2)
			return -(m_2_PI * fixed_x);
		else if (m_PI_2 <= fixed_x && fixed_x < 3.0 * m_PI_2)
			return m_2_PI * fixed_x - 2;
		else return 4 - m_2_PI * fixed_x;
	}

	double Functions::saw(double x)
	{
        double Phase = m_1_PI * x - 2.0 * std::floor(x * m_1_2PI) - 1.0;
        return Phase;
    }

	double Functions::square(double x)
	{
		double fixed_x = x - std::floor(x * m_1_2PI) * m_2PI;
		return (fixed_x * m_1_PI > 1) ? -1 : 1;
	}

	double Functions::modifiedSine(double x, double level)
	{
		double Sine = sine(x) * level;
		return std::round(Sine) / level;
	}

	double Functions::modifiedSaw(double x)
	{
		double Saw = -saw(x) * m_PI_2;
		if (std::abs(Saw) > 0.5) Saw = Saw > 0.0 ? 1.0 : -1.0;
		
		return Saw;
	}

	namespace EquationSolver
	{
		inline double NewtonMethod::getDerivative(double x)
		{
			double Fxdx = function(x + dx);
			double Fx = function(x);
			double Dy = Fxdx - Fx;
			double Dx = dx;
			double Derivative = Dy / Dx;
			return Derivative;
		}

		inline double NewtonMethod::getZeroIntersect(double x)
		{
			return -function(x) / getDerivative(x) + x;
		}
		
		double NewtonMethod::operator()(double begin, double tolerance, unsigned n)
		{
			tolerance = std::fabs(tolerance);
			double x = begin;
			for (auto i = n; i > 0; i--)
			{
				double pre_x = x;
				x = getZeroIntersect(x);
				if (pre_x == x || std::isnan(x) || std::isinf(x)) x = pre_x + dx;
				double fx = std::fabs(function(x));
				if (fx < tolerance) return x;
			}
			return x;
		}

		double BisectionMethod::operator()(double x0, double x1, double tolerance, unsigned N)
		{
			double xa = 0;
			for (unsigned i = 0; i < N; i++)
			{
				xa = (x0 + x1) * 0.5;
				double ya = function(xa);
				if (function(x0) * ya < 0.0) x1 = xa;
				else x0 = xa;
				if (std::fabs(ya) < tolerance) break;
			}
			return xa;
		}

		double lagrangePolynomial(double x, const std::span<QPointF> &points)
		{
			// static_assert(std::is_same_v<qreal, double>);

			double y = 0, y_i;
			for (auto i = points.begin(); i != points.end(); i++)
			{
				y_i = 1.0;
				for (auto j = i; j != points.end(); j++)
				{
					if (i != j)
					{
						y_i *= (x - j->x()) / (i->x() - j->x());
					}
					y += y_i * i->y();
				}
			}
			return y;
		}
	}

    QPointF norm(const QPointF &point)
    {
        if (point.isNull()) return QPointF(0.0, 0.0);
		double length = std::sqrt(point.x() * point.x() + point.y() * point.y());
		return QPointF(point.x() / length, point.y() / length);
    }
    
	QPointF rotate(const QPointF &point, double angleRad)
    {
        double cosA = std::cos(angleRad);
		double sinA = std::sin(angleRad);
		return QPointF(point.x() * cosA - point.y() * sinA,
					   point.x() * sinA + point.y() * cosA);
    }
}
