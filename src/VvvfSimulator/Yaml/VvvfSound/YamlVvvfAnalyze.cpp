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

// YamlVvvfAnalyze.cpp
// Version 1.9.1.1

// Internal Includes
#include "../../Exception.hpp"
#include "../../Logging.hpp"
#include "YamlVvvfAnalyze.hpp"
// Standard Library Includes
#include <fstream>
// Package Includes
#include <QApplication>
#include <QByteArray>
#include <QMessageBox>
#include <QMessageLogContext>
#include <QObject>
#include <QtLogging>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>

namespace NAMESPACE_YAMLVVVFSOUND
{
	YamlVvvfSoundData::YamlVvvfSoundData(RflCppFormats format, std::filesystem::path Path)
	{
		const rfl::Result<YamlVvvfSoundData> result = load(format, Path);
		try { *this = result.value(); }
		catch (const std::runtime_error& error)
		{
			const std::exception_ptr ePtr = std::current_exception();
			static const QMessageLogContext context(__FILE__, __LINE__, Q_FUNC_INFO, "VvvfSimulator::Exception::ConstructorError");
			const QString message = QObject::tr("Failed to load YamlVvvfSoundData from file: %1\n\n%2").arg(
				QString::fromStdString(Path.string()),
				QString::fromStdString(error.what()));
			const QVariantMap details = {
				{"format", static_cast<char>(format)},
				{"longFormat", QObject::tr(VvvfSimulator::Yaml::toString(format))},
				{"path", QString::fromStdString(Path.string())},
				{"error", QString::fromStdString(error.what())}	
			};
			throw VvvfSimulator::Exception::VvvfException(message.toUtf8().constData(), &context, &details, ePtr);
		}
	}
	
	rfl::Result<rfl::Nothing> YamlVvvfSoundData::save (RflCppFormats format, std::filesystem::path Path) const
	{
	switch (format)
		{
		case RflCppFormats::JSON:
			return rfl::json::save(Path.string(), *this, rfl::json::pretty);
			break;
		default: // RflCppFormats::YAML
			return rfl::yaml::save(Path.string(), *this);
		}
	}
	rfl::Result<YamlVvvfSoundData> YamlVvvfSoundData::load(RflCppFormats format, std::filesystem::path Path)
	{
		switch (format)
		{
		case RflCppFormats::JSON:
			return rfl::json::load<YamlVvvfSoundData>(Path.string());
			break;
		default: // RflCppFormats::YAML
			return rfl::yaml::load<YamlVvvfSoundData>(Path.string());
		}
	}
}
