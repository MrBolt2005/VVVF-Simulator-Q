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

// Internal Includes
#include "InternalMath.hpp" // To specialize
// Package Includes
#include <QtMath>

std::function<double(double)> NAMESPACE_VVVF::InternalMath::Functions::_sine = static_cast<double(*)(double)>(std::sin);

double NAMESPACE_VVVF::InternalMath::Functions::saw(double x)
{
	double fixed_x = x - std::floor(x * m_1_2PI) * m_2PI;
	if (0 <= fixed_x && fixed_x < m_PI_2)
		return -(m_2_PI * fixed_x);
	else if (m_PI_2 <= fixed_x && fixed_x < 3.0 * m_PI_2)
		return m_2_PI * fixed_x - 2;
	else return 4 - m_2_PI * fixed_x;
}

double NAMESPACE_VVVF::InternalMath::Functions::square(double x)
{
	double fixed_x = x - std::floor(x * m_1_2PI) * m_2PI;
	return (fixed_x * m_1_PI > 1) ? -1 : 1;
}

double NAMESPACE_VVVF::InternalMath::Functions::modifiedSine(double x, double level)
{
	double Sine = sine(x) * level;
	return std::round(Sine) / level;
}

double NAMESPACE_VVVF::InternalMath::Functions::modifiedSaw(double x)
{
	double Saw = -saw(x) * m_PI_2;
	if (std::abs(Saw) > 0.5) Saw = Saw > 0.0 ? 1.0 : -1.0;
	
	return Saw;
}

bool NAMESPACE_VVVF::InternalMath::Functions::setSineFunction(SineFunctions newSetting)
{
	switch (newSetting)
	{
	case SineFunctions::StandardLibrary:
		_sine = static_cast<double(*)(double)>(std::sin);
		return true;
		break;
	case SineFunctions::InternalFastSine:
		_sine = sineFast;
		return true;
		break;
	case SineFunctions::QFastSin:
		if constexpr (std::is_same<qreal, double>::value)
			_sine = qFastSin;
		else
			_sine = [&](double x) { return static_cast<double>(qFastSin(static_cast<qreal>(x))); };
		return true;
		break;
	default:
		return false;
	};
}

SineFunctions NAMESPACE_VVVF::InternalMath::Functions::getSineFunction()
{
    if (_sine.target<double(*)(double)>() == static_cast<double(*)(double)>(std::sin))
    {
        return SineFunctions::StandardLibrary;
    }
    else if (_sine.target<double(*)(double)>() == sineFast)
    {
        return SineFunctions::InternalFastSine;
    }
    else if (_sine.target<double(*)(double)>() == /* QtFastSine function pointer */)
    {
        return SineFunctions::QFastSin;
    }
    else
    {
        return SineFunctions::GetFail;
    }
}

double NAMESPACE_VVVF::InternalMath::EquationSolver::NewtonMethod::calculate(double begin, double tolerance, unsigned n)
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
