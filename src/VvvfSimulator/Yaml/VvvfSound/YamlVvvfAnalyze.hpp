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

				class PulseHarmonic
				{
				public:
					enum class PulseHarmonicType:uint_fast8_t
					{
						Sine, Saw, Square
					};

					double Harmonic = 3.0, Amplitude = 0.2, InitialPhase = 0;
					bool IsHarmonic Proportional = true, IsAmplitudeProportional = true;
					PulseHarmonicType Type = PulseHarmonicType::Sine;
				};

				class DiscreteTimeConfiguration
				{
					int Steps = 2;
				public:
					enum class DiscreteTimeMode:uint_fast8_t
					{
						Left, Middle, Right
					};

					DiscreteTimeMode Mode = DiscreteTimeMode::Middle;

					bool getEnabled();
					bool setEnabled(bool enable);
					int  getSteps();
					bool setSteps(int steps);
					bool setStepsAndEnable(int steps);
				};

				enum class PulseDataKey:uint_fast8_t
				{
					Bipolar, PulseWidth
				};

				PulseTypeName PulseType;
				// PulseCount can now be a non-integer number, for the funs/lolz	~
				double PulseCount = 1.0;

			//
			// Alternative Modes
			//
				static constexpr PulseAlternative__Default = 0;
				int_fast8_t Alternative = PulseAlternative__Default;

			//
			// Flat Configurations
			//
				bool Shift = false, Square = false;

			//
			// Compare Base Wave
			//
				BaseWaveType BaseWave = BaseWaveType::Sine;

			//
			// Discrete Time Configuration
			//
				DiscreteTimeConfiguration DiscreteTime;

			//
			// Compare Wave Harmonics
			//
				std::vector<PulseHarmonic> PulseHarmonics{};

			//
			// Pulse Custom Variable
			//
				std::map<PulseDataKey, PulseDataValue> PulseData;

				YamlPulseMode(const YamlPulseMode& YPM);
			};

			class YamlAmplitude
			{
			public:
				class AmplitudeParameter
				{
					struct AmplitudeTableEntry
					{
						double Frequency{}, Amplitude{};
					};
				public:
					enum class AmplitudeMode:uint_fast8_t
					{
						Linear, LinearPolynomial, InverseProportional, Exponential, Sine, Table
					};

					double StartFrequency  = -1.0,  StartAmplitude = -1.0,
					         EndFrequency  = -1.0,    EndAmplitude = -1.0,
						   CurveChangeRate =  0.0, CutOffAmplitude = -1.0,
						     MaxAmplitude  = -1.0,    Polynomial   =  0.0;
				};
			}
		};
	};
}
