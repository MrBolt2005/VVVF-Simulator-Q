#include "YamlVehicleSoundAnalyze.hpp"

// Standard Library
#include <array>
#include <cmath>

namespace VvvfSimulator::Yaml::VehicleAudioSetting::YamlVehicleSoundAnalyze
{
	YamlVehicleSoundData::FilterArray YamlVehicleSoundData::getFilters(double sampleFreq)
	{
		FilterArray nFilters;

		for (const auto& soundFilter : Filters)
		{
			TIIRFilterParams params = {};
			params.ProtoType = BUTTERWORTH; // Example: Butterworth prototype
			params.NumPoles = 2;            // 2nd-order filter
			params.Ripple = 0.0;            // Not used for Butterworth
			params.StopBanddB = 0.0;        // Not used for Butterworth
			params.Gamma = 0.0;             // Not used for Butterworth

			// Set filter-specific parameters
			switch (soundFilter.Type)
			{
			case SoundFilter::FilterType::LowPassFilter:
				params.IIRPassType = iirLPF;
				params.OmegaC = soundFilter.Frequency / (sampleFreq * 0.5); // Normalize frequency
				break;
			
			case SoundFilter::FilterType::HighPassFilter:
				params.IIRPassType = iirHPF;
				params.OmegaC = soundFilter.Frequency / (sampleFreq * 0.5); // Normalize frequency
				break;
			
			case SoundFilter::FilterType::BandPassFilter:
				params.IIRPassType = iirBPF;
				params.OmegaC = soundFilter.Frequency / (sampleFreq * 0.5); // Normalize frequency
				params.BW = soundFilter.Frequency / soundFilter.Q;          // Bandwidth
				break;

			case SoundFilter::FilterType::NotchFilter:
				params.IIRPassType = iirNOTCH;
				params.OmegaC = soundFilter.Frequency / (sampleFreq * 0.5); // Normalize frequency
				params.BW = soundFilter.Frequency / soundFilter.Q;          // Bandwidth
				break;

			case SoundFilter::FilterType::AllPassFilter:
				params.IIRPassType = iirALLPASS;
				params.OmegaC = soundFilter.Frequency / (sampleFreq * 0.5); // Normalize frequency
				break;

			default: // case SoundFilter::FilterType::PeakingEQ:
				params.IIRPassType = iirBPF; // Band-Pass as a base for Peaking EQ
				params.OmegaC = soundFilter.Frequency / (sampleFreq * 0.5); // Normalize frequency
				params.BW = soundFilter.Frequency / soundFilter.Q;          // Bandwidth
				params.dBGain = soundFilter.Gain;                           // Gain in dB
				//break;
			}

			// Calculate the filter coefficients
			/*
			TIIRCoeff coeff = CalcIIRFilterCoeff(params);
			nFilters.push_back(coeff);
			*/
			nFilters.emplace_back(CalcIIRFilterCoeff(params));
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