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

// Data/Vvvf.hpp
// Version 1.10.0.0

// Standard Library
#include <cinttypes>
#include <map>
#include <optional>
#include <string>
#include <vector>
// Internal
#include "Util.hpp"

namespace VvvfSimulator::Data
{
    struct Vvvf
    {
        // Forward declarations for Analyze
        enum class PulseMode : uint_fast8_t;
        enum class WaveForm : uint_fast8_t;
        struct ControlFrequencyData;

        /// Main VVVF sound/control data structure
        /// PWM level (2 or 3)
        int level = 2;

        /// Jerk (acceleration/braking ramp) settings
        struct JerkSettings
        {
            struct Jerk
            {
                struct JerkInfo
                {
                    double frequencyChangeRate = 60.0; // Hz/s
                    double maxControlFrequency = 60.0; // Hz
                };

                JerkInfo on;
                JerkInfo off;
            };

            Jerk braking;
            Jerk accelerating;
        };
        JerkSettings jerkSetting;

        /// Minimum base frequency settings
        struct MinimumBaseFrequency
        {
            double accelerating = -1.0;
            double braking = -1.0;
        };
        MinimumBaseFrequency minimumFrequency;

        /// Pulse control pattern (one entry per frequency range)
        struct PulseControl
        {
            double controlFrequencyFrom = -1.0;
            double rotateFrequencyFrom = -1.0;
            double rotateFrequencyBelow = -1.0;
            bool enableFreeRunOn = true;
            bool stuckFreeRunOn = false;
            bool enableFreeRunOff = true;
            bool stuckFreeRunOff = false;
            bool enableNormal = true;

            /// Function value for parametric curves
            struct FunctionValue
            {
                enum class FunctionType : uint_fast8_t
                {
                    Proportional,
                    Inv_Proportional,
                    Pow2_Exponential,
                    Sine
                };

                FunctionType type = FunctionType::Proportional;
                double start = 0.0;
                double startValue = 0.0;
                double end = 1.0;
                double endValue = 100.0;
                double degree = 2.0;
                double curveRate = 0.0;
            };

            /// Pulse mode configuration
            struct Pulse
            {
                enum class PulseTypeName : uint_fast8_t
                {
                    ASYNC,
                    SYNC,
                    CHM,
                    SHE,
                    HO,
                    ΔΣ
                };

                enum class PulseAlternative : int_fast8_t
                {
                    Default = 0,
                    Alt1,
                    Alt2,
                    Alt3,
                    Alt4,
                    Alt5,
                    Alt6,
                    Alt7,
                    Alt8,
                    Alt9,
                    Alt10,
                    Alt11,
                    Alt12,
                    Alt13,
                    Alt14,
                    Alt15,
                    Alt16,
                    Alt17,
                    Alt18,
                    Alt19,
                    Alt20,
                    Alt21,
                    Alt22,
                    Alt23,
                    Alt24,
                    Alt25,
                    CP = -2,
                    Square = -1
                };

                enum class BaseWaveType : uint_fast8_t
                {
                    Sine,
                    Saw,
                    Square,
                    ModifiedSineA,
                    ModifiedSineB,
                    ModifiedSawA,
                    SV,
                    DPWM30,
                    DPWM60C,
                    DPWM60P,
                    DPWM60N,
                    DPWM120P,
                    DPWM120N
                };

                enum class PulseDataKey : uint_fast8_t
                {
                    Phase,
                    Bipolar,
                    PulseWidth,
                    UpdateFrequency
                };

                struct PulseDataValue
                {
                    enum class PulseDataValueMode : uint_fast8_t
                    {
                        Const,
                        Moving
                    };

                    PulseDataValueMode mode = PulseDataValueMode::Const;
                    double constant = 0.0;
                    FunctionValue movingValue;
                };

                struct PulseHarmonic
                {
                    enum class PulseHarmonicType : uint_fast8_t
                    {
                        Sine,
                        Saw,
                        Square
                    };

                    double harmonic = 3.0;
                    double amplitude = 0.2;
                    double initialPhase = 0.0;
                    PulseHarmonicType type = PulseHarmonicType::Sine;
                    bool isHarmonicProportional = true;
                    bool isAmplitudeProportional = true;
                };

                struct DiscreteTimeConfiguration
                {
                    enum class DiscreteTimeMode : uint_fast8_t
                    {
                        Left,
                        Middle,
                        Right
                    };

                    bool enabled = false;
                    int steps = 3;
                    DiscreteTimeMode mode = DiscreteTimeMode::Middle;
                };

                struct CarrierWaveConfiguration
                {
                    enum class CarrierWaveType : uint_fast8_t
                    {
                        Triangle,
                        Saw,
                        Sine
                    };

                    enum class CarrierWaveOption : uint_fast8_t
                    {
                        RaiseStart,
                        FallStart,
                        TopStart,
                        BottomStart
                    };

                    CarrierWaveType type = CarrierWaveType::Triangle;
                    CarrierWaveOption option = CarrierWaveOption::RaiseStart;
                };

                PulseTypeName pulseType = PulseTypeName::ASYNC;
                int pulseCount = -1;
                PulseAlternative alternative = PulseAlternative::Default;
                BaseWaveType baseWave = BaseWaveType::Sine;
                DiscreteTimeConfiguration discreteTime;
                CarrierWaveConfiguration carrierWave;
                std::vector<PulseHarmonic> pulseHarmonics;
                std::map<PulseDataKey, PulseDataValue> pulseData;
            };
            Pulse pulseMode;

            /// Amplitude control
            struct AmplitudeValue
            {
                enum class AmplitudeMode : uint_fast8_t
                {
                    Const,
                    Moving,
                    Table
                };

                struct AmplitudeTableEntry
                {
                    double frequency = 0.0;
                    double amplitude = 0.0;
                };

                AmplitudeMode mode = AmplitudeMode::Const;
                double constant = 1.0;
                FunctionValue movingValue;
                std::vector<AmplitudeTableEntry> table;
                bool tableInterpolation = false;
            };
            AmplitudeValue amplitude;

            /// Async modulation control
            struct AsyncControl
            {
                struct RandomModulation
                {
                    struct RandomValue
                    {
                        enum class RandomValueMode : uint_fast8_t
                        {
                            Const,
                            Moving
                        };

                        RandomValueMode mode = RandomValueMode::Const;
                        double constant = 0.0;
                        FunctionValue movingValue;
                    };

                    RandomValue range;
                    RandomValue interval;
                };
                RandomModulation random;

                struct CarrierFrequency
                {
                    enum class CarrierFrequencyMode : uint_fast8_t
                    {
                        Const,
                        Moving,
                        Periodic,
                        Table
                    };

                    struct VibratoValue
                    {
                        enum class BaseWaveType : uint_fast8_t
                        {
                            Sine,
                            Triangle,
                            Square,
                            SawUp,
                            SawDown
                        };

                        double highest = 1000.0;
                        double lowest = 100.0;
                        double interval = 1.0;
                        BaseWaveType baseWave = BaseWaveType::Sine;
                        bool continuous = false;
                    };

                    struct CarrierTableEntry
                    {
                        double controlFrequency = 0.0;
                        double carrierFrequency = 0.0;
                    };

                    CarrierFrequencyMode mode = CarrierFrequencyMode::Const;
                    double constant = 1000.0;
                    FunctionValue movingValue;
                    VibratoValue vibrato;
                    std::vector<CarrierTableEntry> table;
                    bool tableInterpolation = false;
                };
                CarrierFrequency carrierWaveData;
            };
            AsyncControl asyncModulationData;
        };

        std::vector<PulseControl> acceleratePattern;
        std::vector<PulseControl> brakingPattern;

        // ===== Data Structure Methods =====
        void sortAcceleratePattern(bool inverse);
        void sortBrakingPattern(bool inverse);
        bool hasCustomPwm() const;
        std::string toString() const;

        // ===== Analysis Methods (ex-Analyze) =====

        /// Get the control data at a specific frequency
        ControlFrequencyData getControlFrequencyData(double controlFrequency) const;

        // Type aliases for pattern matching
        using AcceleratePattern = Vvvf::PulseControl;
        using BrakingPattern = Vvvf::PulseControl;

        /// Get the current accelerate control level
        std::optional<AcceleratePattern> getCurrentAcceleratePattern(double controlFrequency) const;

        /// Get the current braking control level
        std::optional<BrakingPattern> getCurrentBrakingPattern(double controlFrequency) const;

        /// Calculate the free run frequency
        double getFreeRunFrequency(double controlFrequency) const;

        /// Get minimum frequency
        double getMinimumFrequency() const;

        /// Get maximum frequency
        double getMaximumFrequency() const;

        /// Check if custom PWM is required at this frequency
        bool requiresCustomPwm(double controlFrequency) const;

        /// Calculate wave amplitude at given control frequency
        double getAmplitude(double controlFrequency) const;

        /// Get the pulse mode at a specific frequency
        PulseMode getPulseMode(double controlFrequency) const;

        /// Get carrier frequency at control frequency
        double getCarrierFrequency(double controlFrequency) const;

        /// Check if frequency is in async mode range
        bool isAsyncMode(double controlFrequency) const;

        /// Calculate bipolar value at frequency
        int getBipolar(double controlFrequency) const;

        // Pulse mode enumeration
        enum class PulseMode : uint_fast8_t
        {
            PWM,
            Async,
            CHM,
            SHE,
            HO
        };

        // Waveform type enumeration
        enum class WaveForm : uint_fast8_t
        {
            Sine,
            Saw,
            Square,
            ModifiedSineA,
            ModifiedSineB,
            ModifiedSawA,
            SV,
            DPWM30,
            DPWM60C,
            DPWM60P,
            DPWM60N,
            DPWM120P,
            DPWM120N
        };

        // Control frequency data structure (used by Analyze)
        struct ControlFrequencyData
        {
            WaveForm waveForm = WaveForm::Sine;
            double amplitude = 0.0;
            double carrierFrequency = 0.0;
            int bipolar = 3;
            PulseMode pulseMode = PulseMode::PWM;
        };

        // Backward compatibility aliases
        using YamlVvvfSoundData = Vvvf;
        using YamlControlData = Vvvf::PulseControl;
        using YamlPulseMode = Vvvf::PulseControl::Pulse;
        using YamlMovingValue = Vvvf::PulseControl::FunctionValue;
    };
}
