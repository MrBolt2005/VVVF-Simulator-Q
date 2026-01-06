#include "YamlVehicleSoundAnalyze.hpp"

// Standard Library
#include <array>
#include <cmath>

namespace VvvfSimulator::Yaml::VehicleAudioSetting::YamlVehicleSoundAnalyze
{
	YamlVehicleSoundData::FilterArray YamlVehicleSoundData::getFilters(float sampleFreq)
	{
		FilterArray nFilters;

		for (const auto& soundFilter : Filters)
		{
			// Set filter-specific parameters
			switch (soundFilter.Type)
			{
			case SoundFilter::FilterType::LowPassFilter:
			{
				auto a_b = DSP::BiquadFilter<float>::calculateLPFCoefficients(soundFilter.Frequency, soundFilter.Q, sampleFreq);
				nFilters.emplace_back(std::move(a_b.first), std::move(a_b.second));
				break;
			}
			case SoundFilter::FilterType::HighPassFilter:
			{
				auto a_b = DSP::BiquadFilter<float>::calculateHPFCoefficients(soundFilter.Frequency, soundFilter.Q, sampleFreq);
				nFilters.emplace_back(std::move(a_b.first), std::move(a_b.second));
				break;
			}
			case SoundFilter::FilterType::BandPassFilter:
			{
				auto a_b = DSP::BiquadFilter<float>::calculateBPFCoefficients(soundFilter.Frequency, soundFilter.Q, sampleFreq);
				nFilters.emplace_back(std::move(a_b.first), std::move(a_b.second));
				break;
			}
			case SoundFilter::FilterType::NotchFilter:
			{
				auto a_b = DSP::BiquadFilter<float>::calculateNotchCoefficients(soundFilter.Frequency, soundFilter.Q, sampleFreq);
				nFilters.emplace_back(std::move(a_b.first), std::move(a_b.second));
				break;
			}
			case SoundFilter::FilterType::AllPassFilter:
			{
				auto a_b = DSP::BiquadFilter<float>::calculateAllPassCoefficients(soundFilter.Frequency, soundFilter.Q, sampleFreq);
				nFilters.emplace_back(std::move(a_b.first), std::move(a_b.second));
				break;
			}
			default: // case SoundFilter::FilterType::PeakingEQ:
			{
				auto a_b = DSP::BiquadFilter<float>::calculatePeakingEQCoefficients(soundFilter.Frequency, soundFilter.Q, soundFilter.Gain, sampleFreq);
				nFilters.emplace_back(std::move(a_b.first), std::move(a_b.second));
			}
			}

			// Calculate the filter coefficients
			/*
			TIIRCoeff coeff = CalcIIRFilterCoeff(params);
			nFilters.push_back(coeff);
			*/
    }

		return nFilters;
	}

	std::vector<YamlVehicleSoundData::HarmonicData> YamlVehicleSoundData::calculatedGearHarmonics(double gear1, double gear2)
	{
		std::vector<HarmonicData> gearHarmonicsList(4);
		const double motor_r = 120.0 / std::pow(2.0, MotorSpec.NP) / 60.0;

		// Sound from Gear 1
		const std::array<double, 4> harmonics
		{
			9.0 * 2 * gear1 / gear2 * 189.0 / 225,
			9.0 * 2 * gear1 / gear2,
			9.0,
			1.0
		};
		for (auto i = 0; i < 4; i++)
		{
			const HarmonicData::HarmonicDataAmplitude amplitude
			{
				0,
				0x0,
				40,
				0.1 * std::pow(1.4, -i),
				0,
				0.1
			};
			const double &k = harmonics[i];
			gearHarmonicsList[i] = HarmonicData(
				motor_r * gear1 * k,
				amplitude,
				-1
			);
		}

		return gearHarmonicsList;
	}
}