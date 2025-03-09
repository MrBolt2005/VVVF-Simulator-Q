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

// CustomPwm.hpp
// Version 1.9.1.1

// Internal Includes
#include "InternalMath.hpp"
#include "Namespace_VVVF.h"
#include "RflCppFormats.hpp"
// Standard Library Includes
#include <cinttypes>
#include <filesystem>
#include <map>
#include <vector>
// Package Includes
#include <QReadWriteLock>
#include <rfl/Result.hpp>

namespace NAMESPACE_VVVF
{
    class CustomPwm
    {
        using RflCppFormats = VvvfSimulator::Yaml::RflCppFormats;
        
        static constexpr int maxPwmLevel = 2;
        QReadWriteLock m_lock;

    protected:
        int32_t m_switchCount = 5;
        double m_modulationIndexDivision = 0.01;
        double m_minimumModulationIndex = 0.0;
        uint32_t m_blockCount = 0;
        
        std::multimap<double, int_fast8_t> m_switchAngleTable{}; // Switching angles must be ordered!
        std::vector<bool> m_polarity{};

    public:
        CustomPwm();
        CustomPwm(const CustomPwm& other);
        // Members left empty on source: m_switchAngleTable, m_polarity
        CustomPwm(CustomPwm&& other);
        CustomPwm(std::filesystem::path Path, RflCppFormats format = RflCppFormats::YAML);

        CustomPwm& operator=(const CustomPwm& other) noexcept;
        // Members left empty on source: m_switchAngleTable, m_polarity
        CustomPwm& operator=(CustomPwm&& other) noexcept;
        
        constexpr int32_t switchCount() const noexcept { return m_switchCount; }
        constexpr double modulationIndexDivision() const noexcept { return m_modulationIndexDivision; }
        constexpr double minimumModulationIndex() const noexcept { return m_minimumModulationIndex; }
        constexpr uint32_t blockCount() const noexcept { return m_blockCount; }
        constexpr const std::multimap<double, int_fast8_t>& constSwitchAngleTable() const noexcept { return m_switchAngleTable; }
        constexpr       std::multimap<double, int_fast8_t>  switchAngleTable()      const noexcept { return std::multimap<double, int_fast8_t>(m_switchAngleTable); }
        constexpr const std::vector<bool>& constPolarity() const noexcept { return m_polarity; }
        constexpr       std::vector<bool>  polarity()      const noexcept { return std::vector<bool>(m_polarity); }

        int_fast8_t getPWM(double M, double X) const;
        
        rfl::Result<rfl::Nothing> save(std::filesystem::path Path, RflCppFormats format = RflCppFormats::YAML) const;
        rfl::Result<CustomPwm>    load(std::filesystem::path Path, RflCppFormats format = RflCppFormats::YAML);
    };
}