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

double NAMESPACE_VVVF::InternalMath::Functions::saw(double x)
{
	double fixed_x = x - std::floor(x * M_1_2PI) * M_2PI;
	if (0 <= fixed_x && fixed_x < M_PI_2)
		return -(M_2_PI * fixed_x);
	else if (M_PI_2 <= fixed_x && fixed_x < 3.0 * M_PI_2)
		return M_2_PI * fixed_x - 2;
	else return 4 - M_2_PI * fixed_x;
}

double NAMESPACE_VVVF::InternalMath::Functions::square(double x)
{
	double fixed_x = x - std::floor(x * M_1_2PI) * M_2PI;
	return (fixed_x * M_1_PI > 1) ? -1 : 1;
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
	case SineFunctions::QtFastSine;
		//
		return true;
		break;
	default:
		return false;
	};
}

SineFunctions NAMESPACE_VVVF::InternalMath::Functions::setSineFunction()
{
	switch (_sine)
	{
	case static_cast<double(*)(double)>(std::sin):
		return StandardLibrary;
		break;
	case sineFast:
		return InternalFastSine;
		break;
	case /**/:
		return QtFastSine;
		break;
	default:
		return GetFail;
	};
}
