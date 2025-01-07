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
#include "../RflCppFormats.hpp"
// Standard Library Includes
#include <cinttypes>
#include <exception>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>
// Package Includes
#include <rfl/Result.hpp>

namespace NAMESPACE_YAMLVVVFSOUND // C++17 Nested Namespace Notation
{
	struct YamlVvvfSoundData
	{
		struct YamlMasconData // ToFromYaml
		{
			struct YamlMasconDataPattern // ToFromYaml
			{
				struct YamlMasconDataPatternMode // ToFromYaml
				{
					double FrequencyChangeRate = 60.0, MaxControlFrequency = 60.0;
				};

				YamlMasconDataPatternMode On, Off;
			};

			YamlMasconDataPattern Braking, Accelerating;
		};

		struct YamlMinSineFrequency // ToFromYaml
		{
			double Accelerating = -1.0, Braking = -1.0;
		};

		struct YamlControlData // Clone, ToFromYaml
		{
			struct YamlMovingValue
			{
				enum class MovingValueType:uint_fast8_t
				{
					Proportional, Inv_Proportional, Pow2_Exponential, Sine
				};

				MovingValueType Type = MovingValueType::Inv_Proportional;
				double Start = 0.0, StartValue = 0.0, End = 1.0, EndValue = 100.0, Degree = 2.0, CurveRate = 0.0;
			};

			struct YamlAsync
			{
				struct RandomModulation
				{
					struct YamlAsyncParameterRandomValue
					{
						enum class YamlAsyncParameterRandomValueMode:uint_fast8_t
						{
							Const, Moving
						};

						YamlAsyncParameterRandomValueMode Mode = YamlAsyncParameterRandomValueMode::Const;
						double Constant{};
						YamlMovingValue MovingValue;
					};

					YamlAsyncParameterRandomValue Range, Interval;
				};

				struct CarrierFrequency
				{
					enum class CarrierFrequencyValueMode:uint_fast8_t
					{
						Const, Moving, Periodic, Table
					};

					struct YamlAsyncParameterCarrierFreqPeriodic
					{
						struct YamlAsyncParameterPeriodicValue 
						{
							enum class YamlAsyncParameterPeriodicMode:uint_fast8_t
							{
								Const, Moving
							};

							YamlAsyncParameterPeriodicMode Mode = YamlAsyncParameterPeriodicMode::Const;
							double Constant = -1.0;
							YamlMovingValue MovingValue;
						};

						YamlAsyncParameterPeriodicValue Highest, Lowest, Interval;
						bool Continuous = true;
					};

					struct YamlAsyncParameterCarrierFreqTableValue
					{
						double ControlFrequencyFrom = -1.0, CarrierFrequency = 1000.0;
						bool   FreeRunStuckAtHere   = false;
					};
					double Constant = -1.0;
					YamlMovingValue MovingValue;
					YamlAsyncParameterCarrierFreqPeriodic PeriodicData;
					// the original .NET code used a class to store the table inside of:
					std::vector<YamlAsyncParameterCarrierFreqTableValue> CarrierFrequencyTable;
					CarrierFrequencyValueMode Mode = CarrierFrequencyValueMode::Const;

					CarrierFrequency(const CarrierFrequency& CF);
				};

				RandomModulation RandomData;
				CarrierFrequency CarrierWaveData;
			};

			struct YamlPulseMode // Clone
			{
			//	Internal components
				class PulseDataValue // ToFromYaml, Static Clone
				{
					enum class PulseDataValueMode:uint_fast8_t
					{
						Const, Moving
					};

					PulseDataValueMode Mode = PulseDataValueMode::Const;
					double Constant = -1.0;
					YamlMovingValue MovingValue;
				};

			//
			// Fundamental Configuration
			//
				enum class PulseTypeName::uint_fast8_t
				{
					ASYNC, SYNC, CHM, SHE, HO,
				};

				enum class BaseWaveType:uint_fast8_t
				{
					Sine, Saw, ModifiedSineA, ModifiedSineB, ModifiedSaw1
				};

				struct PulseHarmonic
				{
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
				//	bool setStepsAndEnable(int steps);
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
				enum class PulseAlternative:int_fast8_t
				{
					Default =  0,
					Shifted = -1,
					CP      = -2,
					Square  = -3,
					Alt1    =  1,
					Alt2    =  2,
					Alt3    =  3,
					Alt4    =  4,
					Alt5    =  5,
					Alt6    =  6,
					Alt7    =  7,
					Alt8    =  8,
					Alt9    =  9,
					Alt10   =  10,
					Alt11   =  11,
					Alt12   =  12,
					Alt13   =  13,
					Alt14   =  14,
					Alt15   =  15,
					Alt16   =  16,
					Alt17   =  17,
					Alt18   =  18,
					Alt19   =  19,
					Alt20   =  20,
					Alt21   =  21,
					Alt22   =  22,
					Alt23   =  23,
					Alt24   =  24,
					Alt25   =  25,
					Alt26   =  26,
					Alt27   =  27,
					Alt28   =  28,
					Alt29   =  29,
					Alt30   =  30,
				}
				PulseAlternative Alternative = PulseAlternative::Default;

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

			struct YamlAmplitude
			{
				struct AmplitudeParameter
				{
					struct AmplitudeTableEntry
					{
						double Frequency{}, Amplitude{};
					};
					enum class AmplitudeMode:uint_fast8_t
					{
						Linear, LinearPolynomial, InverseProportional, Exponential, Sine, Table
					};

					double StartFrequency  = -1.0,  StartAmplitude = -1.0,
					         EndFrequency  = -1.0,    EndAmplitude = -1.0,
						   CurveChangeRate =  0.0, CutOffAmplitude = -1.0,
						     MaxAmplitude  = -1.0,    Polynomial   =  0.0;
					std::vector<AmplitudeTableEntry> AmplitudeTable{};
					AmplitudeMode Mode               = AmplitudeMode::Linear;
					bool DisableRangeLimit           = false,
					     AmplitudeTableInterpolation = false;
				};

				AmplitudeParameter Default, PowerOn, PowerOff;
			};

			double ControlFrequencyFrom = -1.0,
			       RotateFrequencyFrom  = -1.0,
				   RotateFrequencyBelow = -1.0;
			bool   EnableFreeRunOn      =  true,
			       StuckFreeRunOn       = false,
				   EnableFreeRunOff     =  true,
			       StuckFreeRunOff      = false,
				   EnableNormal         =  true;

			YamlPulseMode PulseMode;
			YamlAmplitude Amplitude;
			YamlAsync     AsyncModulationData;
		};

		int_fast8_t Level = 2;
		YamlMasconData MasconData;
		YamlMinSineFrequency MinimumFrequency;
		std::vector<YamlControlData> AcceleratePattern{}, BrakingPattern{};
		void sortAcceleratePattern(bool inverse);
		void sortBrakingPattern   (bool inverse);
		bool hasCustomPwm         ();

		YamlVvvfSoundData(RflCppFormats format, std::filesystem::path Path);
		
		rfl::Result<Nothing>           save(RflCppFormats format, std::filesystem::path Path) const;

		rfl::Result<YamlVvvfSoundData> load(RflCppFormats format, std::filesystem::path Path);
	};
}
