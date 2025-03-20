#pragma once

// Packages
#include <QSerialPort>
// Internal
#include "../GenerateRealTimeCommon.hpp"
#include "../../../Vvvf/Struct.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"

namespace VvvfSimulator::Generation::Audio::VvvfSound::RealTime
{
	int calculate(
		const YamlVvvfSoundData &sound,
		RealTimeParameter &param,
		QSerialPort &serial
	);
}