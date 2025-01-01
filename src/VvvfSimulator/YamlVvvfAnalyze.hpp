#ifndef YAMLVVVFANALYZE_HPP
#define YAMLVVVFANALYZE_HPP

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

// YamlVvvfAnalyze.hpp
// Version 1.9.1.1

// Internal Includes
#include "Namespace_YamlVvvfSound.h"
// Standard Library Includes
#include <cinttypes>
#include <filesystem>
#include <map>
#include <string>
#include <vector>
// Package Includes
#include <yaml_cpp/yaml.h>

namespace VvvfSimulator::Yaml::VvvfSound // C++17 Nested Namespace Notation
{
	class YamlVvvfSoundData
	{
		class YamlMasconData // ToFromYaml
		{
			class YamlMasconDataPattern // ToFromYaml
			{
				struct YamlMasconDataPatternMode
				{
					double FrequencyChangeRate = 60.0, MaxControlFrequency = 60.0;
				};

			public:
				YamlMasconDataPatternMode On, Off;
			};
		};
	};
}
