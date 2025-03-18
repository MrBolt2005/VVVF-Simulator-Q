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

// YamlMasconMidi.hpp
// Version 1.9.1.1

// Standard Library
#include <cinttypes>
#include <expected>
// Packages
#include <libremidi/libremidi.hpp>
#include <libremidi/reader.hpp>
#include <QString>
// Internal
#include "../../GUI/Mascon/LoadMidi.qml.hpp"
#include "Namespace_YamlMasconControl.h"
#include "./YamlMasconAnalyze.hpp"

namespace NAMESPACE_YAMLMASCONCONTROL::YamlMasconMidi
{
	using NAMESPACE_YAMLMASCONCONTROL::YamlMasconAnalyze::YamlMasconData;
	using MidiLoadData = VvvfSimulator::GUI::Mascon::LoadMidi::MidiLoadData;
	struct NoteEventSimple
	{
		struct NoteEventSimpleData
		{
			double time;
			int note;
			bool isNoteOn;
		};

				NoteEventSimpleData On, Off;
	};

	std::expected<YamlMasconData, QString> convert(const MidiLoadData& loadData);

	// Helper function to convert ticks to milliseconds
	constexpr double ticks_to_ms(int ticks, float division, uint32_t tempo)
	{
		return (static_cast<double>(ticks) * static_cast<double>(tempo)) / (static_cast<double>(division) * 1000.0);
	}

	std::vector<NoteEventSimple> getTimeLine(const libremidi::reader& midiData, int track_num);
}