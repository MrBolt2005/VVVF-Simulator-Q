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

// Vvvf/CustomPwm.cpp
// Version 1.10.0.0

#include "CustomPwm.hpp"
#include <fstream>
#include <cstring>
#include <algorithm>
#include <cmath>

namespace VvvfSimulator::Vvvf::CustomPwm
{
	// ===== CustomPwmTable Implementation =====

	CustomPwmTable::CustomPwmTable(const uint8_t* data, size_t size)
	{
		parseBinaryData(data, size);
	}

	void CustomPwmTable::parseBinaryData(const uint8_t* data, size_t size)
	{
		if (size < 21) return; // Minimum: 1 + 8 + 8 + 4 = 21 bytes header

		size_t offset = 0;

		// Read header
		switchCount = data[offset++];

		memcpy(&modulationIndexDivision, data + offset, sizeof(double));
		offset += sizeof(double);

		memcpy(&minimumModulationIndex, data + offset, sizeof(double));
		offset += sizeof(double);

		memcpy(&blockCount, data + offset, sizeof(uint32_t));
		offset += sizeof(uint32_t);

		// Read blocks
		blocks.resize(blockCount);
		for (uint32_t i = 0; i < blockCount; ++i) {
			if (offset >= size) break;

			ModulationBlock& block = blocks[i];
			block.startLevel = data[offset++];
			block.switchAngles.resize(switchCount);

			for (uint8_t j = 0; j < switchCount; ++j) {
				if (offset + sizeof(uint8_t) + sizeof(double) > size) break;

				SwitchAngle& sa = block.switchAngles[j];
				sa.output = data[offset++];
				memcpy(&sa.angle, data + offset, sizeof(double));
				offset += sizeof(double);
			}
		}
	}

	std::optional<CustomPwmTable> CustomPwmTable::loadFromFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file) return std::nullopt;

		size_t size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<uint8_t> buffer(size);
		if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
			return std::nullopt;

		return CustomPwmTable(buffer.data(), size);
	}

	std::optional<CustomPwmTable> CustomPwmTable::loadFromResource(const std::string& resourcePath)
	{
		// TODO: Implement Qt resource loading
		// For now, treat as file path
		return loadFromFile(resourcePath);
	}

	int CustomPwmTable::getPwm(double M, double X) const
	{
		if (!isValid()) return 0;

		// Find block index based on modulation index
		int index = static_cast<int>((M - minimumModulationIndex) / modulationIndexDivision);
		index = std::clamp(index, 0, static_cast<int>(blockCount) - 1);

		const ModulationBlock& block = blocks[index];
		return getPwm(block.switchAngles, X, block.startLevel);
	}

	int CustomPwmTable::getPwm(const std::vector<SwitchAngle>& angles, double X, uint8_t startLevel)
	{
		using namespace InternalMath;

		// Normalize angle to [0, 2π)
		X = std::fmod(X, Functions::M_2_PI);
		if (X < 0) X += Functions::M_2_PI;

		// Determine orthant (quadrant in 90° sectors)
		int orthant = static_cast<int>(X / Functions::M_PI_2);
		double angle = std::fmod(X, Functions::M_PI_2);

		// Mirror angle for odd orthants
		if (orthant & 0x01) {
			angle = Functions::M_PI_2 - angle;
		}

		// Find PWM level by comparing with switch angles
		int pwm = startLevel;
		for (const auto& sa : angles) {
			if (sa.angle <= angle) {
				pwm = sa.output;
			} else {
				break;
			}
		}

		// Invert for orthants 2 and 3
		if (orthant > 1) {
			pwm = MAX_PWM_LEVEL - pwm;
		}

		return pwm;
	}

	// ===== CustomPwmPresets Implementation =====

	bool CustomPwmPresets::loaded = false;
	bool CustomPwmPresets::loading = false;
	std::map<CustomPwmPresets::PresetId, std::shared_ptr<CustomPwmTable>> CustomPwmPresets::cache;

	std::string CustomPwmPresets::getResourcePath(PresetId id)
	{
		// Map preset IDs to resource paths
		// These will match the embedded Qt resources
		static const std::map<PresetId, std::string> pathMap = {
			{PresetId::L2Chm3Default, ":/switchangle/L2Chm3Default.bin"},
			{PresetId::L2Chm3Alt1, ":/switchangle/L2Chm3Alt1.bin"},
			{PresetId::L2Chm3Alt2, ":/switchangle/L2Chm3Alt2.bin"},
			{PresetId::L2Chm5Default, ":/switchangle/L2Chm5Default.bin"},
			{PresetId::L2Chm5Alt1, ":/switchangle/L2Chm5Alt1.bin"},
			{PresetId::L2Chm5Alt2, ":/switchangle/L2Chm5Alt2.bin"},
			{PresetId::L2Chm5Alt3, ":/switchangle/L2Chm5Alt3.bin"},
			{PresetId::L2Chm7Default, ":/switchangle/L2Chm7Default.bin"},
			{PresetId::L2Chm7Alt1, ":/switchangle/L2Chm7Alt1.bin"},
			{PresetId::L2Chm7Alt2, ":/switchangle/L2Chm7Alt2.bin"},
			{PresetId::L2Chm7Alt3, ":/switchangle/L2Chm7Alt3.bin"},
			{PresetId::L2Chm7Alt4, ":/switchangle/L2Chm7Alt4.bin"},
			{PresetId::L2Chm7Alt5, ":/switchangle/L2Chm7Alt5.bin"},
			{PresetId::L2Chm9Default, ":/switchangle/L2Chm9Default.bin"},
			{PresetId::L2Chm9Alt1, ":/switchangle/L2Chm9Alt1.bin"},
			{PresetId::L2Chm9Alt2, ":/switchangle/L2Chm9Alt2.bin"},
			{PresetId::L2Chm9Alt3, ":/switchangle/L2Chm9Alt3.bin"},
			{PresetId::L2Chm9Alt4, ":/switchangle/L2Chm9Alt4.bin"},
			{PresetId::L2Chm9Alt5, ":/switchangle/L2Chm9Alt5.bin"},
			{PresetId::L2Chm9Alt6, ":/switchangle/L2Chm9Alt6.bin"},
			{PresetId::L2Chm9Alt7, ":/switchangle/L2Chm9Alt7.bin"},
			{PresetId::L2Chm9Alt8, ":/switchangle/L2Chm9Alt8.bin"},
			{PresetId::L2Chm11Default, ":/switchangle/L2Chm11Default.bin"},
			{PresetId::L2Chm11Alt1, ":/switchangle/L2Chm11Alt1.bin"},
			{PresetId::L2Chm11Alt2, ":/switchangle/L2Chm11Alt2.bin"},
			{PresetId::L2Chm11Alt3, ":/switchangle/L2Chm11Alt3.bin"},
			{PresetId::L2Chm11Alt4, ":/switchangle/L2Chm11Alt4.bin"},
			{PresetId::L2Chm11Alt5, ":/switchangle/L2Chm11Alt5.bin"},
			{PresetId::L2Chm11Alt6, ":/switchangle/L2Chm11Alt6.bin"},
			{PresetId::L2Chm11Alt7, ":/switchangle/L2Chm11Alt7.bin"},
			{PresetId::L2Chm11Alt8, ":/switchangle/L2Chm11Alt8.bin"},
			{PresetId::L2Chm11Alt9, ":/switchangle/L2Chm11Alt9.bin"},
			{PresetId::L2Chm11Alt10, ":/switchangle/L2Chm11Alt10.bin"},
			{PresetId::L2Chm13Default, ":/switchangle/L2Chm13Default.bin"},
			{PresetId::L2Chm13Alt1, ":/switchangle/L2Chm13Alt1.bin"},
			{PresetId::L2Chm13Alt2, ":/switchangle/L2Chm13Alt2.bin"},
			{PresetId::L2Chm13Alt3, ":/switchangle/L2Chm13Alt3.bin"},
			{PresetId::L2Chm13Alt4, ":/switchangle/L2Chm13Alt4.bin"},
			{PresetId::L2Chm13Alt5, ":/switchangle/L2Chm13Alt5.bin"},
			{PresetId::L2Chm13Alt6, ":/switchangle/L2Chm13Alt6.bin"},
			{PresetId::L2Chm13Alt7, ":/switchangle/L2Chm13Alt7.bin"},
			{PresetId::L2Chm13Alt8, ":/switchangle/L2Chm13Alt8.bin"},
			{PresetId::L2Chm15Default, ":/switchangle/L2Chm15Default.bin"},
			{PresetId::L2Chm15Alt1, ":/switchangle/L2Chm15Alt1.bin"},
			{PresetId::L2Chm15Alt2, ":/switchangle/L2Chm15Alt2.bin"},
			{PresetId::L2Chm15Alt3, ":/switchangle/L2Chm15Alt3.bin"},
			{PresetId::L2Chm15Alt4, ":/switchangle/L2Chm15Alt4.bin"},
			{PresetId::L2Chm15Alt5, ":/switchangle/L2Chm15Alt5.bin"},
			{PresetId::L2Chm15Alt6, ":/switchangle/L2Chm15Alt6.bin"},
			{PresetId::L2Chm15Alt7, ":/switchangle/L2Chm15Alt7.bin"},
			{PresetId::L2Chm15Alt8, ":/switchangle/L2Chm15Alt8.bin"},
			{PresetId::L2Chm15Alt9, ":/switchangle/L2Chm15Alt9.bin"},
			{PresetId::L2Chm15Alt10, ":/switchangle/L2Chm15Alt10.bin"},
			{PresetId::L2Chm15Alt11, ":/switchangle/L2Chm15Alt11.bin"},
			{PresetId::L2Chm15Alt12, ":/switchangle/L2Chm15Alt12.bin"},
		};

		auto it = pathMap.find(id);
		return (it != pathMap.end()) ? it->second : "";
	}

	void CustomPwmPresets::loadPreset(PresetId id)
	{
		std::string path = getResourcePath(id);
		if (path.empty()) return;

		auto table = CustomPwmTable::loadFromResource(path);
		if (table) {
			cache[id] = std::make_shared<CustomPwmTable>(*table);
		}
	}

	std::shared_ptr<CustomPwmTable> CustomPwmPresets::getPreset(PresetId id)
	{
		// Check cache first
		auto it = cache.find(id);
		if (it != cache.end()) {
			return it->second;
		}

		// Load on demand
		loadPreset(id);
		return cache[id]; // May be nullptr if load failed
	}

	void CustomPwmPresets::preloadAll()
	{
		if (loaded || loading) return;
		loading = true;

		// Load all presets
		// In production, this would be done asynchronously
		for (int i = static_cast<int>(PresetId::L2Chm3Default); 
		     i <= static_cast<int>(PresetId::L2Chm15Alt12); ++i) {
			loadPreset(static_cast<PresetId>(i));
		}

		loaded = true;
		loading = false;
	}

} // namespace VvvfSimulator::Vvvf::CustomPwm
