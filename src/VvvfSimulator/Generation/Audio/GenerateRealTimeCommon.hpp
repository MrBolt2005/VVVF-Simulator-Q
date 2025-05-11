#pragma once

// Packages
#include <QAudioDevice>
#include <QPointer>
#include <QString>
// Internal
#include "../Motor/GenerateMotorCore.hpp"
#include "../../Vvvf/Struct.hpp"
#include "../../Yaml/VehicleAudioSetting/YamlVehicleSoundAnalyze.hpp"
#include "../../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"

namespace VvvfSimulator::Generation::Audio::GenerateRealTimeCommon
{
	using Vvvf::Struct::VvvfValues;
	using Yaml::VvvfSound::YamlVvvfSoundData;
	using Generation::Motor::GenerateMotorCore::Motor;
	using Yaml::VehicleAudioSetting::YamlVehicleSoundAnalyze::YamlVehicleSoundData;
	
	struct RealTimeParameter
	{
		double freqChangeRate = 0;
		VvvfValues control;
		YamlVvvfSoundData vvvfSoundData;
		Motor motor;
		YamlVehicleSoundData vehicleSoundData;
		QPointer<QAudioDevice> audioDevice;
		bool isBraking = false;
		bool quit = false;
		bool isFreeRunning = false;

		#define TEMP sizeof(VvvfSimulator::Generation::Audio::RealTimeParameter)
	};

	int realTimeFrequencyControl(VvvfValues &control, const RealTimeParameter &param, double dt);
}