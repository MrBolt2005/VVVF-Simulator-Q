#include "YamlMasconMidi.hpp"

// Standard Library
#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <vector>

namespace NAMESPACE_YAMLMASCONCONTROL::YamlMasconMidi
{
	std::expected<YamlMasconData, QString> convert(const MidiLoadData & loadData)
	{
		using namespace NAMESPACE_YAMLMASCONCONTROL::YamlMasconMidi;

		// Convert MIDI data
		const std::filesystem::path fsPath(loadData.path.toStdU16String());
		std::ifstream file(fsPath, std::ios::binary | std::ios::in);
		if (!file.is_open()) return std::unexpected(QObject::tr("Failed to open MIDI file: %1").arg(fsPath.root_path().native()));

		// Read the file into a vector of uint8_t
		const std::vector<uint8_t> fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		
		libremidi::reader reader;
		const auto parseResult = reader.parse(fileData);
		if (parseResult == libremidi::reader::parse_result::invalid)
			return std::unexpected(QObject::tr("Failed to parse MIDI file: %1").arg(fsPath.root_path().native()));

		auto converted_Constructs = getTimeLine(reader, loadData.track);
		YamlMasconData mascon_Data;

		double totalTime = 0.0;

		for (int j = 0; j < loadData.priority; j++)
		{
			double pre_event_time = 0.0;
			std::vector<int> selected_Data;
			for (int i = 0; i < converted_Constructs.size(); i++)
			{
				const NoteEventSimple &data = converted_Constructs[i];

				if (data.On.time < pre_event_time) continue;

				const double initialWait = data.On.time - pre_event_time;
				const double playWait = data.Off.time - data.On.time;

				pre_event_time = data.Off.time;
				selected_Data.emplace_back(i);

				if (loadData.priority != j + 1) continue;

				// set initial
				mascon_Data.points.emplace_back(YamlMasconData::YamlMasconDataPoint(4 * i, 0, -1, false, true));
				// wait initial
				mascon_Data.points.emplace_back(YamlMasconData::YamlMasconDataPoint(4 * i + 1, 0, initialWait, false, true));
				// set play
				const double frequency = 440.0 * std::pow(2.0, (data.On.note - 69) / 12.0);
				mascon_Data.points.emplace_back(YamlMasconData::YamlMasconDataPoint(4 * i + 2, frequency, -1, false, true));
				// wait play
				mascon_Data.points.emplace_back(YamlMasconData::YamlMasconDataPoint(4 * i + 2, 0, playWait, false, true));

				totalTime += playWait + initialWait;
			}

			// Remove elements in reverse order to avoid invalidating indices
			for (auto it = selected_Data.rbegin(); it != selected_Data.rend(); ++it)
			{
				converted_Constructs.erase(converted_Constructs.begin() + *it);
			}
		}
	}

	std::vector<NoteEventSimple> getTimeLine(const libremidi::reader& midiData, int track_num)
	{
		// Get the selected track; throws if the track number is out of range
		const auto& track = midiData.tracks.at(track_num);

		std::vector<NoteEventSimple> events;

		// Get the division (ticks per quarter note)
		float division = midiData.ticksPerBeat;

		// Tempo map for converting ticks to time, default tempo is 500000 microseconds per quarter note
		std::map<int, uint32_t> tempoMap;
		tempoMap[0] = 500000;

		// Iterate over the events in the track to build the tempo map
		for (const auto& event : track) {
			if (event.m.is_meta_event() && event.m.get_meta_event_type() == libremidi::meta_event_type::TEMPO_CHANGE)
			{
				const int &tick = event.tick;
				// Extract the tempo from the meta event bytes
				// Will be converted to double during math operations; namely, by ticks_to_ms
				const uint32_t tempo = ((((uint32_t)event.m.bytes[3]) << 16) + (event.m.bytes[4] << 8) + event.m.bytes[5]);
				tempoMap[tick] = tempo;
			}
		}

		// Iterate over the events in the track to process Note events
		for (const auto& event : track) {
			if (event.m.get_message_type() == libremidi::message_type::NOTE_ON)
			{
				const int &tick = event.tick;
				int duration_ticks = 0;
				// Find the corresponding Note Off event to calculate the duration
				for (const auto& off_event : track)
				{
					if (off_event.m.get_message_type() == libremidi::message_type::NOTE_OFF &&
						off_event.m.bytes[1] == event.m.bytes[1] && off_event.tick > tick) {
						duration_ticks = off_event.tick - tick;
						break;
					}
				}
				// Get the current tempo for the tick
				const auto current_tempo = tempoMap.lower_bound(tick)->second;

				// Convert ticks to milliseconds for Note On and Note Off events
				const double time_on = ticks_to_ms(tick, division, current_tempo);
				const double time_off = ticks_to_ms(tick + duration_ticks, division, current_tempo);

				// Create Note On event data
				const NoteEventSimple::NoteEventSimpleData Note_ON_Data =
				{
					time_on,
					event.m.bytes[1], // MIDI note number
					true
				};

				// Create Note Off event data
				const NoteEventSimple::NoteEventSimpleData Note_OFF_Data =
				{
					time_off,
					event.m.bytes[1], // MIDI note number
					false
				};

				// Add the Note event to the list
				events.emplace_back(NoteEventSimple(Note_ON_Data, Note_OFF_Data));
			}
		}

		// Sort events by the time of Note On events
		std::sort(events.begin(), events.end(), [](const NoteEventSimple& a, const NoteEventSimple& b)
		{
			return a.On.time < b.On.time;
		});

		return events;
	}
}