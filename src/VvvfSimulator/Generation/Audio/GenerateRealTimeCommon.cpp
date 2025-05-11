#include "GenerateRealTimeCommon.hpp"

// Standard Library
#include <algorithm>
// Internal
#include "../../Yaml/MasconControl/YamlMasconAnalyze.hpp"

namespace VvvfSimulator::Generation::Audio::GenerateRealTimeCommon
{
	int realTimeFrequencyControl(VvvfValues &control, const RealTimeParameter &param, double dt)
	{
		control.brake = param.isBraking;
		control.masconOff = param.isFreeRunning;

		double sinNewAngleFreq = control.sinAngleFreq + param.freqChangeRate * dt;
		if (sinNewAngleFreq < 0.0) sinNewAngleFreq = 0.0;

		if (!control.freeRun)
		{
			if (control.allowSineTimeChange)
			{
				if (sinNewAngleFreq != 0.0)
				{
					const double amp = control.sinAngleFreq / sinNewAngleFreq;
					control.sinTime *= amp;
				}
				else control.sinTime = 0.0;
			}

			control.controlFrequency = control.getSinFreq();
			control.sinAngleFreq = sinNewAngleFreq;
		}

	if (param.quit) return 0;

		{
			// Max Frequency Set (Almost Same Thing Exists on YamlVvvfWave)
			double maxVoltageFreq;
			Yaml::VvvfSound::YamlVvvfSoundData::YamlMasconData::YamlMasconDataPattern pattern
			 = control.brake
			 ? param.vvvfSoundData.MasconData.Accelerating
			 : param.vvvfSoundData.MasconData.Braking;

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
					control.controlFrequency = maxVoltageFreq;
			}
		}

		if (!control.masconOff) // mascon on
		{
			if (!control.freeRun) control.controlFrequency = control.getSinFreq();
			else
			{
				const double freqChange = control.freeFreqChange * dt;
				const double finalFreq = control.controlFrequency + freqChange;

				if (control.getSinFreq() <= finalFreq)
				{
					control.controlFrequency = control.getSinFreq();
					control.freeRun = false;
				}
				else
				{
					control.controlFrequency = finalFreq;
					control.freeRun = true;
				}
			}
		}
		else
		{
			const double freqChange = control.freeFreqChange * dt;
			const double finalFreq = control.controlFrequency - freqChange;
			control.controlFrequency = std::max(finalFreq, 0.0);
			control.freeRun = true;
		}

		return -1;
	}
}