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

// Vvvf/CustomPwm.hpp
// Version 1.10.0.0 - Custom PWM (CHM/SHE) switch angle table loader

#pragma once

// Standard Library
#include <vector>
#include <cinttypes>
#include <filesystem>
#include <optional>
#include <string>
#include <memory>
#include <map>
// Packages
#include <QString>
// Internal
#include "InternalMath.hpp"
#include "../Outcome.hpp"

namespace VvvfSimulator::Vvvf::CustomPwm
{
	/// Switch angle entry (angle + output level)
	struct SwitchAngle
	{
		double angle = 0.0;
		uint8_t output = 0;
	};

	/// Single modulation index block containing switch angles
	struct ModulationBlock
	{
		uint8_t startLevel = 0;
		std::vector<SwitchAngle> switchAngles;
	};

	/// Complete custom PWM lookup table
	class CustomPwmTable
	{
	public:
		CustomPwmTable() = default;

		/// Load from binary stream (original .bin format)
		explicit CustomPwmTable(const uint8_t* data, size_t size);

		/// Load from file path
		static Outcome::QStringResult<CustomPwmTable> loadFromBin(const QString& path);

		/// Get PWM output level for given modulation index and phase angle
		/// @param M Modulation index (0.0 - ~1.3)
		/// @param X Phase angle (radians, 0 - 2π)
		/// @return PWM level (0, 1, or 2 for 2-level)
		int getPwm(double M, double X) const;

		/// Static variant taking switch angle array directly
		static int getPwm(const std::vector<SwitchAngle>& angles, double X, uint8_t startLevel);

		// Getters
		uint8_t getSwitchCount() const { return switchCount; }
		double getModulationIndexDivision() const { return modulationIndexDivision; }
		double getMinimumModulationIndex() const { return minimumModulationIndex; }
		uint32_t getBlockCount() const { return blockCount; }
		bool isValid() const { return blockCount > 0 && switchCount > 0; }

	private:
		static constexpr int MAX_PWM_LEVEL = 2;

		uint8_t switchCount = 0;
		double modulationIndexDivision = 0.0;
		double minimumModulationIndex = 0.0;
		uint32_t blockCount = 0;
		std::vector<ModulationBlock> blocks;

		void parseBinaryData(const uint8_t* data, size_t size);
	};

	/// Preset manager for embedded switch angle tables
	namespace CustomPwmPresets
	{
		/// Preset identifiers matching upstream
		enum class PresetId
		{
			L2Chm3Default,
			L2Chm3Alt1,
			L2Chm3Alt2,
			L2Chm5Default,
			L2Chm5Alt1,
			L2Chm5Alt2,
			L2Chm5Alt3,
			L2Chm7Default,
			L2Chm7Alt1,
			L2Chm7Alt2,
			L2Chm7Alt3,
			L2Chm7Alt4,
			L2Chm7Alt5,
			L2Chm9Default,
			L2Chm9Alt1,
			L2Chm9Alt2,
			L2Chm9Alt3,
			L2Chm9Alt4,
			L2Chm9Alt5,
			L2Chm9Alt6,
			L2Chm9Alt7,
			L2Chm9Alt8,
			L2Chm11Default,
			L2Chm11Alt1,
			L2Chm11Alt2,
			L2Chm11Alt3,
			L2Chm11Alt4,
			L2Chm11Alt5,
			L2Chm11Alt6,
			L2Chm11Alt7,
			L2Chm11Alt8,
			L2Chm11Alt9,
			L2Chm11Alt10,
			L2Chm13Default,
			L2Chm13Alt1,
			L2Chm13Alt2,
			L2Chm13Alt3,
			L2Chm13Alt4,
			L2Chm13Alt5,
			L2Chm13Alt6,
			L2Chm13Alt7,
			L2Chm13Alt8,
			L2Chm15Default,
			L2Chm15Alt1,
			L2Chm15Alt2,
			L2Chm15Alt3,
			L2Chm15Alt4,
			L2Chm15Alt5,
			L2Chm15Alt6,
			L2Chm15Alt7,
			L2Chm15Alt8,
			L2Chm15Alt9,
			L2Chm15Alt10,
			L2Chm15Alt11,
			L2Chm15Alt12,
			// SHE presets would go here
		};

		/// Get preset table by ID (lazy loading)
		std::shared_ptr<CustomPwmTable> getPreset(PresetId id);

		/// Preload all presets asynchronously
		void preloadAll();

		/// Check if presets are loaded
		bool isLoaded();
	};

} // namespace VvvfSimulator::Vvvf::CustomPwm
