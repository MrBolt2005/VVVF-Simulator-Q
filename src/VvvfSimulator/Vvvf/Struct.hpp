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
#include "../Random/xoshiro256.hpp"
#include "../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"
// Standard Library Includes
#include <cinttypes>
#include <map>
#include <optional>
#include <stdexcept>
// Package Includes
#include <QString>
#include <QVector>

namespace NAMESPACE_VVVF::Struct
{
	using namespace NAMESPACE_VVVF::InternalMath;
	using YamlPulseMode = NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData::YamlControlData::YamlPulseMode;
  	using PulseTypeName = YamlPulseMode::PulseTypeName;
		using PulseAlternative = YamlPulseMode::PulseAlternative; // Commented out as it is not a type name
		using PulseDataKey = YamlPulseMode::PulseDataKey;
	
	#pragma pack(1) // sizeof(int_fast8_t)
	struct WaveValues
	{
		static int_fast8_t nullValue;
		int_fast8_t U, V, W;

		          WaveValues() = default;
		constexpr WaveValues(int_fast8_t u, int_fast8_t v, int_fast8_t w) :
			U(u), V(v), W(w)
		{}

		constexpr std::optional<int_fast8_t&> optAt(size_t i)
		{
			switch (i)
			{
			case 0:
				return U;
				break;
			case 1:
				return V;
				break;
			case 2:
				return W;
				break;
			default:
				return std::nullopt;
			}
		}

		constexpr std::optional<const int_fast8_t&> optAt(size_t i) const
		{
			switch (i)
			{
			case 0:
				return U;
				break;
			case 1:
				return V;
				break;
			case 2:
				return W;
				break;
			default:
				return std::nullopt;
			}
		}

		constexpr int_fast8_t& operator[](size_t i)
		{
			const auto value = optAt(i);
			if (!value)
				throw std::out_of_range("Index out of range");
			return *value;
		}

		constexpr const int_fast8_t& operator[](size_t i) const
		{
			const auto value = optAt(i);
			if (!value)
				throw std::out_of_range("Index out of range");
			return *value;
		}

		constexpr bool operator==(const WaveValues &other) const noexcept
		{
			return this->U == other.U && this->V == other.V && this->W == other.W;
		}

		constexpr bool operator!=(const WaveValues &other) const noexcept
		{
			return !(*this == other);
		}
	};

	struct CarrierFreq
	{
		double baseFrequency, range, interval;

		          CarrierFreq() = default;
		constexpr CarrierFreq(double bF, double r, double i) :
			baseFrequency(bF), range(r), interval(i)
		{}
	};

	struct VvvfValues
	{
		double controlFrequency  = 0.0;
		double freeFreqChange    = 0.0;
		bool   brake             = false;
		bool   freeRun           = false;
		bool   masconOff         = false;

		bool allowSineTimeChange =  true;
		bool allowRandomFreqMove =  true;

		void resetControlValues();

		VvvfSimulator::Random::xoshiro256ss rnd{};

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

		constexpr double getSinFreq() const noexcept
		{
			return sinAngleFreq * m_1_2PI;
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

		VvvfValues();

		QStringList getPulseName() const;
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
			return getPulseAlternatives(pulseMode.PulseType, pulseMode.PulseCount, level);
		}
	}
}

#endif // STRUCT_HPP
