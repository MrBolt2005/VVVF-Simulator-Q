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

// YamlMasconAnalyze.hpp
// Version 1.9.1.1

// Standard Library
#include <algorithm>
#include <vector>
// Internal
#include "Namespace_YamlMasconControl.h"
#include "../../Vvvf/InternalMath.hpp"
#include "../../Vvvf/Struct.hpp"

#if __cplusplus >= 202002L
#define CXX20_CONSTEXPR constexpr
#else
#define CXX20_CONSTEXPR
#endif

namespace NAMESPACE_YAMLMASCONCONTROL::YamlMasconAnalyze
{
	using NAMESPACE_VVVF::Struct::VvvfValues;
	using NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;

	struct YamlMasconData
	{
		struct YamlMasconDataPoint
		{
			int order = 0;
			double rate = 0.0; // Hz/s
			double duration = 0.0; // S
			bool brake = false;
			bool masconOn = true;

			constexpr YamlMasconDataPoint() = default;
			constexpr YamlMasconDataPoint(int Order) : order(Order) {}
			constexpr YamlMasconDataPoint(const YamlMasconDataPoint &other) = default;
			constexpr YamlMasconDataPoint(YamlMasconDataPoint &&other) = default;

			constexpr YamlMasconDataPoint& operator=(const YamlMasconDataPoint &other) = default;
			constexpr YamlMasconDataPoint& operator=(YamlMasconDataPoint &&other) = default;
		};
		
		std::vector<YamlMasconDataPoint> points{};

		constexpr       YamlMasconData() = default;
		CXX20_CONSTEXPR YamlMasconData(const YamlMasconData &other) : points(other.points) {}
		CXX20_CONSTEXPR YamlMasconData(YamlMasconData &&other) : points(std::move(other.points)) {}

		CXX20_CONSTEXPR double getEstimatedSteps(double sampleTime) const
		{
			double totalDuration = 0.0;
			for (const auto &point : points) totalDuration += point.duration > 0.0 ? point.duration : 0.0;

			return totalDuration / sampleTime;
		}

		CXX20_CONSTEXPR YamlMasconData& sort()
		{
			std::sort(this->points.begin(), this->points.end(), [&](const YamlMasconDataPoint &a, const YamlMasconDataPoint &b)
			{
				return a.order < b.order;
			});
			return *this;
		}

		CXX20_CONSTEXPR YamlMasconData sort() const
		{
			auto ymd(*this);
			std::sort(ymd.points.begin(), ymd.points.end(), [&](const YamlMasconDataPoint &a, const YamlMasconDataPoint &b)
			{
				return a.order < b.order;
			});
			return ymd;
		}
	};

	struct YamlMasconDataCompiled
	{
		struct YamlMasconDataCompiledPoint
		{
			double startTime = 0.0;
			double endTime = 0.0;
			double startFrequency = 0.0;
			double endFrequency = 0.0;
			bool isMasconOn = true;
			bool isAccel = true;

			constexpr YamlMasconDataCompiledPoint() = default;
			constexpr YamlMasconDataCompiledPoint(const YamlMasconDataCompiledPoint &other) = default;
			constexpr YamlMasconDataCompiledPoint(YamlMasconDataCompiledPoint &&other) = default;
			constexpr YamlMasconDataCompiledPoint(double startTime, double endTime, double startFrequency, double endFrequency, bool isMasconOn, bool isAccel)
				: startTime(startTime), endTime(endTime), startFrequency(startFrequency), endFrequency(endFrequency), isMasconOn(isMasconOn), isAccel(isAccel) {}
		};

		std::vector<YamlMasconDataCompiledPoint> points{};

		constexpr       YamlMasconDataCompiled() = default;
		CXX20_CONSTEXPR YamlMasconDataCompiled(const YamlMasconDataCompiled &other) : points(other.points) {}
		CXX20_CONSTEXPR YamlMasconDataCompiled(YamlMasconDataCompiled &&other) : points(std::move(other.points)) {}
		CXX20_CONSTEXPR YamlMasconDataCompiled(YamlMasconData ymd)
		{
			using YamlMasconDataPoint = YamlMasconData::YamlMasconDataPoint;
			
			std::sort(ymd.points.begin(), ymd.points.end(), [&](const YamlMasconDataPoint &a, const YamlMasconDataPoint &b)
			{
				return a.order < b.order;
			});

			double currentTime = 0;
			double currentFrequency = 0;
			for (const auto &ymdPoint : ymd.points)
			{
				if (ymdPoint.duration == -1.0)
				{
					currentFrequency = ymdPoint.rate;
					continue;
				}

				const double &deltaTime = ymdPoint.duration;
				      double  deltaFrequency = deltaTime * ymdPoint.rate;
				if (ymdPoint.brake) deltaFrequency *= -1.0;

				if (deltaTime == 0.0) continue;

				points.emplace_back(
					currentTime,
					currentTime + deltaTime,
					currentFrequency,
					currentFrequency + deltaFrequency,
					ymdPoint.masconOn,
					!ymdPoint.brake
				);

				currentTime += deltaTime;
				currentFrequency += deltaFrequency;
			}
		}

		CXX20_CONSTEXPR double getEstimatedSteps(double sampleTime) const
		{
			const double &totalTime = points.back().endTime;
			return totalTime / sampleTime;
		}

		// YamlMasconControl.cs
		CXX20_CONSTEXPR ssize_t getPointAtNum(double time) const
		{
			if (time < points.front().startTime || points.back().endTime < time) return -1;

			ssize_t E_L = 0, E_R = points.size() - 1, pos = (E_R - E_L) / 2 + E_L;
			while (true)
			{
				if (points[pos].startTime <= time && time < points[pos].endTime) return pos;
				if (points[pos].startTime < time) E_L = pos + 1;
				else if (points[pos].startTime > time) E_R = pos - 1;
				//if (E_L > E_R) return -1;
				pos = (E_R - E_L) / 2 + E_L;
			}

			return pos;
		}

		CXX20_CONSTEXPR YamlMasconDataCompiledPoint getPointAtData(double time) const
		{
			return points.at(getPointAtNum(time));
		}

		CXX20_CONSTEXPR double getFreqAt(double time, double initial) const
		{
			const auto &selected = getPointAtData(time);

			const double A_frequency = (selected.endFrequency - selected.startFrequency) / (selected.endTime - selected.startTime);
			const double   frequency = A_frequency * (time - selected.startTime) + selected.startFrequency;

			return frequency + initial;
		}

		CXX20_CONSTEXPR bool checkForFreqChange(VvvfValues &control, const YamlVvvfSoundData &soundData, double timeDelta) const
		{
			using namespace NAMESPACE_VVVF::InternalMath;

			const double &currentTime = control.generationCurrentTime;
			auto dataAt = getPointAtNum(currentTime);
			if (dataAt < 0) return false;
			const auto *target = &(points.at(dataAt));
			const auto *nextTarget = dataAt + 1 < points.size() ? &points.at(dataAt + 1) : nullptr;
			const auto *previousTarget = dataAt - 1 >= 0 ? &points.at(dataAt - 1) : nullptr;

			bool braking = target->isAccel, isMasconOn = target->isMasconOn;
			double forceOnFrequency = -1.0;

			if (!isMasconOn && previousTarget != nullptr) braking = !previousTarget->isAccel;

			if (nextTarget != nullptr && control.freeRun && nextTarget->isMasconOn)
			{
				double masconOnFrequency = getFreqAt(target->endTime, 0.0);
				double freqPerSec, freqGoto;
				if (!nextTarget->isAccel)
				{
					freqPerSec = soundData.MasconData.Braking.On.FrequencyChangeRate;
					freqGoto = soundData.MasconData.Braking.On.MaxControlFrequency;
				}
				else
				{
					freqPerSec = soundData.MasconData.Accelerating.On.FrequencyChangeRate;
					freqGoto = soundData.MasconData.Accelerating.On.MaxControlFrequency;
				}

				double targetFrequency = std::min(masconOnFrequency, freqGoto);
				double requireTime = targetFrequency / freqPerSec;
				if (target->endTime - requireTime < control.generationCurrentTime)
				{
					isMasconOn = true;
					braking = !nextTarget->isAccel;
					forceOnFrequency = targetFrequency;
				}
			}

			double newSineFrequency = std::max(getFreqAt(currentTime, 0.0), 0.0);

			control.brake = braking;
			control.masconOff = !isMasconOn;
			control.freeRun = target != nullptr && !target->isMasconOn;

			control.sinAngleFreq = newSineFrequency * m_2PI;
			if (control.allowSineTimeChange)
				control.sinTime *= newSineFrequency == 0.0 ? 0.0 : control.getSinFreq() / newSineFrequency;

			if (forceOnFrequency != -1.0)
			{
				control.sinAngleFreq = forceOnFrequency * m_2PI;
				if (control.allowSineTimeChange)
					control.sinTime *= forceOnFrequency == 0.0 ? 0.0 : control.getSinFreq() / forceOnFrequency;
			}



			//This is also the core of the controlling. This should never be changed.

			// Max Frequency Set (Almost Same Thing Exists on YamlVvvfWave)
			{
				using YamlMasconDataPattern = YamlVvvfSoundData::YamlMasconData::YamlMasconDataPattern;
				
				double maxVoltageFreq;
				const YamlMasconDataPattern &pattern = control.brake ? soundData.MasconData.Braking : soundData.MasconData.Accelerating;

				if (!control.masconOff)
				{
					control.freeFreqChange = pattern.On.FrequencyChangeRate;
					maxVoltageFreq = pattern.On.MaxControlFrequency;
					if (control.freeRun && control.controlFrequency > maxVoltageFreq)
						control.controlFrequency = control.getSinFreq();
				}
				else
				{
					control.freeFreqChange = pattern.Off.FrequencyChangeRate;
					maxVoltageFreq = pattern.Off.MaxControlFrequency;
					if (control.freeRun && control.controlFrequency > maxVoltageFreq)
						control.controlFrequency = control.getSinFreq();
				}	
			}

			if (!control.masconOff) // mascon on
			{
				if (!control.freeRun) control.controlFrequency = control.getSinFreq();
				else
				{
					const double freqChange = control.freeFreqChange * timeDelta;
					const double finalFreq = control.controlFrequency + freqChange;
					control.controlFrequency = finalFreq > control.getSinFreq() ? control.getSinFreq() : finalFreq;
				}
			}
			else
			{
				const double freqChange = control.freeFreqChange * timeDelta;
				const double finalFreq = control.controlFrequency - freqChange;
				control.controlFrequency = std::max(finalFreq, 0.0);
			}

			control.generationCurrentTime += timeDelta;



			return target != nullptr;
		}
	};
}