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
#include "YamlVvvfAnalyze.hpp"
// Standard Library Includes
#include <fstream>
// Package Includes
#include <QMessageBox>
#include <QtLogging>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <rfl/yaml.hpp>

namespace NAMESPACE_YAMLVVVFSOUND
{
	YamlVvvfSoundData(RflCppFormats format, std::filesystem::path Path)
	{
		const rfl::Result<YamlVvvfSoundData> result = load(format, Path);
		if (result)
			*this = result.value();
		else
		{
			qCritical("%s: %s\n", typeid(YamlVvvfSoundData::YamlVvvfSoundData(RflCppFormats, std::filesystem::path)).name(), result.error().value());
			QMessageBox::critical(nullptr, QObject::tr(Error), result.error().value());
		}
	}
	
	rfl::Result<Nothing> YamlVvvfSoundData::save (RflCppFormats format, std::filesystem::path Path) const
	{
	switch (format)
		{
		case RflCppFormats::JSON:
			return rfl::json::save(Path, *this, rfl::json::pretty);
			break;
		default: // RflCppFormats::YAML
			return rfl::yaml::save(Path, *this);
		}
	}
}
