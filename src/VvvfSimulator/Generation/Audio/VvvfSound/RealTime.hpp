#pragma once

// Standard Library
#include <exception>
#include <variant>
// Packages
#include <QSerialPort>
// Internal
#include "../GenerateRealTimeCommon.hpp"
#include "../../../Outcome.hpp"
#include "../../../Vvvf/Struct.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"

namespace VvvfSimulator::Generation::Audio::VvvfSound::RealTime
{
	Outcome::Result<void, std::variant<QSerialPort::SerialPortError, std::exception_ptr>> calculate(
		const Yaml::VvvfSound::YamlVvvfSoundData &sound,
		GenerateRealTimeCommon::RealTimeParameter &param,
		QSerialPort &serial
	);
}