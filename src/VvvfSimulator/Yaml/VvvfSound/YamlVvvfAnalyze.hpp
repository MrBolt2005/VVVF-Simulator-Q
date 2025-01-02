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
				struct YamlMasconDataPatternMode // ToFromYaml
				{
					double FrequencyChangeRate = 60.0, MaxControlFrequency = 60.0;
				};

			public:
				YamlMasconDataPatternMode On, Off;
			};

		public:
			YamlMasconDataPattern Braking, Accelerating;
		};

		struct YamlMinSineFrequency // ToFromYaml
		{
			double Accelerating = -1.0, Braking = -1.0;
		};

	public:
		class YamlControlData // Clone, ToFromYaml
		{
			class YamlMovingValue
			{
				enum class MovingValueType:uint_fast8_t
				{
					Proportional, Inv_Proportional, Pow2_Exponential, Sine
				};

			public:
				MovingValueType Type = MovingValueType::Inv_Proportional;
				double Start = 0.0, StartValue = 0.0, End = 1.0, EndValue = 100.0, Degree = 2.0, CurveRate = 0.0;
			};

			class YamlAsync
			{
				class RandomModulation
				{
					class YamlAsyncParameterRandomValue
					{
						enum class YamlAsyncParameterRandomValueMode:uint_fast8_t
						{
							Const, Moving
						};

					public:
						YamlAsyncParameterRandomValueMode Mode = YamlAsyncParameterRandomValueMode::Const;
						double Constant{};
						YamlMovingValue MovingValue;
					};

				public:
					YamlAsyncParameterRandomValue Range, Interval;
				};

				class CarrierFrequency
				{
					enum class CarrierFrequencyValueMode:uint_fast8_t
					{
						Const, Moving, Periodic, Table
					};

					class YamlAsyncParameterCarrierFreqPeriodic
					{
						class YamlAsyncParameterPeriodicValue 
						{
							enum class YamlAsyncParameterPeriodicMode:uint_fast8_t
							{
								Const, Moving
							};

						public:
							YamlAsyncParameterPeriodicMode Mode = YamlAsyncParameterPeriodicMode::Const;
							double Constant = -1.0;
							YamlMovingValue MovingValue;
						};

					public:
						YamlAsyncParameterPeriodicValue Highest, Lowest, Interval;
						bool Continuous = true;
					};

					struct YamlAsyncParameterCarrierFreqTableValue
					{
						double ControlFrequencyFrom = -1.0, CarrierFrequency = 1000.0;
						bool   FreeRunStuckAtHere   = false;
					};
				public:
					double Constant = -1.0;
					YamlMovingValue MovingValue;
					YamlAsyncParameterCarrierFreqPeriodic PeriodicData;
					// the original .NET code used a class to store the table inside of:
					std::vector<YamlAsyncParameterCarrierFreqTableValue> CarrierFrequencyTable;
					CarrierFrequencyValueMode Mode = CarrierFrequencyValueMode::Const;

					CarrierFrequency(const CarrierFrequency& CF);
				};

			public:
				RandomModulation RandomData;
				CarrierFrequency CarrierWaveData;
			};

		public:
			class YamlPulseMode // Clone
			{
			//	Internal components
				class PulseDataValue // ToFromYaml, Static Clone
				{
					enum class PulseDataValueMode:uint_fast8_t
					{
						Const, Moving
					};

				public:
					PulseDataValueMode Mode = PulseDataValueMode::Const;
					double Constant = -1.0;
					YamlMovingValue MovingValue;
				};

			//
			// Fundamental Configuration
			//
			public:
				enum class PulseTypeName::uint_fast8_t
				{
					ASYNC, SYNC, CHM, SHE, HO,
				};

				enum class BaseWaveType:uint_fast8_t
				{
					Sine, Saw, ModifiedSineA, ModifiedSineB, ModifiedSaw1
				};
			};
		};
	};
}
