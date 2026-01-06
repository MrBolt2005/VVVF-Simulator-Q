#pragma once

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

// YamlVvvfUtil.hpp
// Version 1.9.1.1

// Standard Library
#include <cinttypes>
// Internal
#include "../../Vvvf/InternalMath.hpp"
#include "Namespace_YamlVvvfSound.h"
#include "YamlVvvfAnalyze.hpp"

namespace NAMESPACE_YAMLVVVFSOUND::YamlVvvfUtil
{
	namespace AutoModulationIndexSolver
	{
		using namespace NAMESPACE_VVVF::InternalMath::EquationSolver;

		struct SolveConfiguration
		{		
			YamlVvvfSoundData soundData;
			double accelEndFrequency;
			double accelMaxVoltage;
			double brakeEndFrequency;
			double brakeMaxVoltage;
			EquationSolverType solverType;
			int maxEffort;
			double precision;
			int tableDivision;
			bool isTableDivisionPerHz;

			bool run();
		};

		struct InternalFunctionParameter
		{
			SolveConfiguration configuration;
			bool isBrakePattern;
			int64_t index;
			double maxFrequency;
			double maxVoltageRate;
		};
	}
}