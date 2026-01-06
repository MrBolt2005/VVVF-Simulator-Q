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

// Calculate.hpp
// Version 1.9.1.1

// Internal Includes
#include "InternalMath.hpp"
#include "Namespace_VVVF.h"
#include "Struct.hpp"
#include "YamlVvvfAnalyze.hpp" // for YamlVvvfSoundData
// Standard Library Includes
#include <array>
#include <cinttypes>
#include <stdexcept>
#include <optional>
// Package Includes
#include <QPointF>

namespace NAMESPACE_VVVF::Calculate
{
	using YamlPulseMode = NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData::YamlControlData::YamlPulseMode;
		using DiscreteTimeConfiguration = YamlPulseMode::DiscreteTimeConfiguration;
	using AmplitudeParameter = NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData::YamlControlData::YamlAmplitude::AmplitudeParameter;
	using CarrierFreq = NAMESPACE_VVVF::Struct::CarrierFreq;
	using VvvfValues = NAMESPACE_VVVF::Struct::VvvfValues;
	using WaveValues = NAMESPACE_VVVF::Struct::WaveValues;
	using PwmCalculateValues = NAMESPACE_VVVF::Struct::PwmCalculateValues;

	//double modifiedSine(double x);
	//double modifiedSaw(double x);
	double getBaseWaveform(const YamlPulseMode& mode, double x, double amplitude, double T, double initialPhase, bool* OK = nullptr);
	constexpr int_fast8_t modulateSignal (double sinValue, double sawValue)
	{
		return sinValue > sawValue ? 1 : 0;
	}

	//
	// Discrete Time
	//
	double discreteTimeLine(double x, int level, DiscreteTimeConfiguration::DiscreteTimeMode mode, bool* OK = nullptr);

	//
	// Pulse Calculation
	//
	int_fast8_t getHOP(double x, double amplitude, int_fast8_t carrier, int_fast8_t width);
	double getAmplitude(const AmplitudeParameter& param, double current);

	//
	// Carrier Freq. Calculation
	//
	double getRandomFrequency(const CarrierFreq& data, VvvfValues& control);
	double getPeriodicFrequency(double lowest, double highest, double intervalTime, bool continuous, VvvfValues& control);

	//
	// VVVF Calculation
	//
	WaveValues calculatePhases(VvvfValues& control, const PwmCalculateValues& value, double addInitial, std::array<std::exception_ptr, 3> *ePtrs = nullptr);
	int_fast8_t calculate3Level(const VvvfValues& control, const PwmCalculateValues& value, double initialPhase);
	int_fast8_t calculate2Level(const VvvfValues& control, const PwmCalculateValues& value, double initialPhase);
}

namespace NAMESPACE_VVVF::SVM
{
	using namespace NAMESPACE_VVVF::InternalMath;
	using WaveValues = NAMESPACE_VVVF::Struct::WaveValues;

	struct FunctionTime
	{
		double T0, T1, T2;

		constexpr FunctionTime() = default;
		constexpr FunctionTime(double t0, double t1, double t2) : T0(t0), T1(t1), T2(t2) {}

		constexpr std::optional<double> operator[](int_fast8_t index) const
		{
			switch (index)
			{
			case 0: return T0;
			case 1: return T1;
			case 2: return T2;
			default: return std::nullopt;
			}
		}

		constexpr FunctionTime operator*(const FunctionTime& value) const
		{
			return FunctionTime(T0 * value.T0, T1 * value.T1, T2 * value.T2);
		}
		
		constexpr FunctionTime operator*(double value) const
		{
			return FunctionTime(T0 * value, T1 * value, T2 * value);
		}
	};

	// ABC voltage/current coordinate system
	struct ABC
	{
		double Ua, Ub, Uc;

		constexpr ABC() = default;
		constexpr ABC(double ua, double ub, double uc) : Ua(ua), Ub(ub), Uc(uc) {}
		constexpr ABC(const WaveValues& values) : Ua(values.U), Ub(values.V), Uc(values.W) {}

		constexpr ABC operator+(double d) const
		{
			return ABC(Ua + d, Ub + d, Uc + d);
		}

		constexpr ABC operator*(double d) const
		{
			return ABC(Ua * d, Ub * d, Uc * d);
		}

		constexpr ABC operator-() const
		{
			return ABC(-Ua, -Ub, -Uc);
		}

		constexpr ABC operator-(const ABC& value) const
		{
			return ABC(Ua - value.Ua, Ub - value.Ub, Uc - value.Uc);
		}
	};

	// Alpha-Beta voltage/current coordinate system
	struct Valbe
	{
		double Ualpha, Ubeta;

		constexpr Valbe() = default;
		constexpr Valbe(double ualpha, double ubeta) : Ualpha(ualpha), Ubeta(ubeta) {}
		constexpr Valbe(const ABC& abc) : Ualpha(abc.Ua), Ubeta((abc.Ua + 2.0 * abc.Ub) * m_SQRT3_2) {}

		constexpr operator QPointF() const
		{
			return QPointF(Ualpha, Ubeta);
		}

		constexpr int_fast8_t estimateSector() const
		{
			int_fast8_t A = Ubeta > 0.0 ? 0 : 1,
			            B = Ubeta - m_SQRT3 * Ualpha > 0.0 ? 0 : 1,
									C = Ubeta + m_SQRT3 * Ualpha > 0.0 ? 0 : 1;
			switch (4 * A + 2 * B + C)
			{
				case 1: return 3;
				case 2: return 1;
				case 3:
				case 4: return 0;
				case 5: return 4;
				case 6: return 6;
				case 7: return 5;
				default: return 2;
			}
		}

		constexpr FunctionTime getFunctionTime(int_fast8_t sector) const
		{
			FunctionTime ft;
			switch (sector)
			{
			case 1:
				{
					ft.T1 = m_SQRT3_2 * Ualpha - 0.5 * Ubeta;
					ft.T2 = Ubeta;
				}
				break;
			case 2:
				{
					ft.T1 = m_SQRT3_2 * Ualpha + 0.5 * Ubeta;
					ft.T2 = 0.5 * Ubeta - m_SQRT3_2 * Ualpha;
				}
				break;
			case 3:
				{
					ft.T1 = Ubeta;
					ft.T2 = -(m_SQRT3_2 * Ualpha + 0.5 * Ubeta);
				}
				break;
			case 4:
				{
					ft.T1 = 0.5 * Ubeta - m_SQRT3_2 * Ualpha;
					ft.T2 = -Ubeta;
				}
				break;
			case 5:
				{
					ft.T1 = -(m_SQRT3_2 * Ualpha + 0.5 * Ubeta);
					ft.T2 = m_SQRT3_2 * Ualpha - 0.5 * Ubeta;
				}
				break;
			case 6:
				{
					ft.T1 = Ubeta;
					ft.T2 = m_SQRT3_2 * Ualpha + 0.5 * Ubeta;
				}
				break;
			default: // Should not happen:
				{
					ft.T1 = 0.0;
					ft.T2 = 0.0;
				}
			}
			ft.T0 = 1.0 - ft.T1 - ft.T2;
			return ft;
		}
	};
}