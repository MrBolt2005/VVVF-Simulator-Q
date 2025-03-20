#pragma once

// Standard Library
#include <vector>
// Packages
#include "iowahills/iir.h"
// Internal
#include "../../Bits/CxxConfig.h"
#include "../../Generation/Motor/GenerateMotorCore.hpp"

namespace VvvfSimulator::Yaml::VehicleAudioSetting::YamlVehicleSoundAnalyze
{
	struct YamlVehicleSoundData
	{
		struct HarmonicData
		{
			struct HarmonicDataAmplitude
			{
				double Start = 0.0;
				double StartValue = 0.0;
				double End = 0.0;
				double EndValue = 0.0;
				double MinimumValue = 0.0;
				double MaximumValue = 0.0;
			};

			struct HarmonicDataRange
			{
				double Start = 0.0;
				double End = -1.0;
			};
			
			double Harmonic = 0.0;
			HarmonicDataAmplitude Amplitude;
			HarmonicDataRange Range;
			double Disappear = 0.0;
		};
	
		struct SoundFilter
		{
			enum class FilterType
			{
				LowPassFilter  = iirLPF,
				HighPassFilter = iirHPF,
				BandPassFilter = iirBPF,
				NotchFilter    = iirNOTCH,
				AllPassFilter  = iirALLPASS,
				PeakingEQ   // = 5
			};

			FilterType Type;
			float Gain, Frequency, Q;

			constexpr SoundFilter(FilterType type, float gain, float frequency, float q) noexcept :
				Type(type), Gain(gain), Frequency(frequency), Q(q) {}
			
			constexpr SoundFilter() noexcept = default;

			constexpr SoundFilter(const SoundFilter &other) noexcept = default;
		};
	
		using MotorSpecification = VvvfSimulator::Generation::Motor::GenerateMotorCore::MotorSpecification;

		std::vector<HarmonicData> GearSound, HarmonicSound;
		std::vector<SoundFilter> Filters =
		{
			SoundFilter(SoundFilter::FilterType::HighPassFilter, -3.0f, 50.0f,  2.0f),
			SoundFilter(SoundFilter::FilterType::LowPassFilter,  -3.0f, 900.0f, 2.0f),
		};
		std::vector<float> ImpulseResponse;
		MotorSpecification MotorSpec;
		double MotorVolumeDb = 1.0;
		double TotalVolumeDb = -2.5;
		bool UseFilters = true, UseConvolutionFilter = true;

		using FilterArray = std::vector<TIIRCoeff>;
		FilterArray getFilters(double sampleFreq);

		std::vector<HarmonicData> calculatedGearHarmonics(double gear1, double gear2);
		void setCalculatedGearHarmonics(double gear1, double gear2)
		{
			GearSound = calculatedGearHarmonics(gear1, gear2);
		}

		CXX20_CONSTEXPR YamlVehicleSoundData() noexcept = default;
		CXX20_CONSTEXPR YamlVehicleSoundData(const YamlVehicleSoundData &other) noexcept = default;
		YamlVehicleSoundData(double gear1, double gear2)
		{
			setCalculatedGearHarmonics(gear1, gear2);
		}
	};
}