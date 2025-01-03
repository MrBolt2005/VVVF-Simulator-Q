#ifndef STRUCT_HPP
#define STRUCT_HPP

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

// Struct.hpp
// Version 1.9.1.1

// Internal Includes
#include "InternalMath.hpp"
#include "Namespace_VVVF.h"
#include "../random/xoshiro256.hpp"
#include "../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"
// Standard Library Includes
#include <cinttypes>
#include <map>
// Package Includes
#include <QVector>

namespace NAMESPACE_VVVF::Struct
{
	#pragma pack(sizeof(int_fast8_t))
	struct WaveValues
	{
		int_fast8_t U, V, W;
	};

	struct CarrierFreq
	{
		double baseFrequency, range, interval;
	};

	struct VvvfValues
	{
		using VvvfSimulator::random::xoshiro256ss;
	
		double controlFrequency  = 0.0;
		double freeFreqChange    = 0.0;
		bool   brake             = false;
		bool   freeRun           = false;
		bool   masconOff         = false;

		bool allowSineTimeChange =  true;
		bool allowRandomFreqMode =  true;

		void resetControlValues();

		xoshiro256ss rnd();

		//--- from vvvf wave calculate
		//sin value definitions
		double sinAngleFreq = 0.0;
		double sinTime      = 0.0;
		//saw value definitions
		double sawAngleFreq             = 1050.0;
		double sawTime                  = 0.0;
		double previousSawRandomFreq    = 0.0;
		double randomFreqPreviousTime   = 0.0;
		double periodicFreqPreviousTime = 0.0;

		constexpr double getSinFreq() const
		{
			return sinAngleFreq * M_1_2PI;
		}

		void resetMathematicValues();

		// Values for Video Generation.
		YamlPulseMode videoPulseMode;
		std::map<PulseDataKey, double> videoPulseData{};
		double videoSineAmplitude{};
		CarrierFreq videoCarrierFrequency;
		double videoSineFrequency{};

		// Values for Check mascon
		double generationCurrentTime = 0.0;
		//
		VvvfValues(const VvvfValues& control);
	};

	struct PwmCalculateValues
	{
		YamlPulseMode pulseMode;
		CarrierFreq carrier = { 100.0, 0.0, 0.0005 };

		std::map<PulseDataKey, double> pulseData{};
		int_fast8_t level{};
		bool none{};

		double amplitude{}, minimumFrequency{};

		PwmCalculateValues(const PwmCalculateValues& values);
	};

	namespace PulseModeConfiguration
	{
		       QVector<double> getAvailablePulseCount(PulseTypeName pulseType, int level);
		       QVector<PulseTypeName> getAvailablePulseType(int level);
		       bool isPulseSquareAvailable (const YamlPulseMode& pulseMode, int level);
		       bool isCompareWaveEditable  (const YamlPulseMode& pulseMode, int level);
		       bool isPulseShiftedAvailable(const YamlPulseMode& pulseMode, int level);
		       QVector<PulseAlternative> getPulseAlternatives(PulseTypeName pulseType, double pulseCount, int level);
		inline QVector<PulseAlternative> getPulseAlternatives(const YamlPulseMode& pulseMode, int level)
		{
			return getPulseAlternatives(pulseMode.pulseType, pulseMode.pulseCount, level);
		}
	}
}

#endif // STRUCT_HPP
