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

// Data/VehicleAudio.hpp
// Version 1.10.0.0 - Train audio configuration

#include <cstdint>
#include "Util.hpp"
#include <vector>
#include <string>

namespace VvvfSimulator::Data {
struct TrainAudio {

    // Motor electrical and mechanical specifications for sound generation
    struct MotorSpecification {
        double V = 220.0;           // Source voltage (V)
        double Rs = 0.45;           // Stator resistance (ohm)
        double Rr = 0.38;           // Rotor resistance (ohm)
        double Ls = 0.012;          // Stator inductance (H)
        double Lr = 0.012;          // Rotor inductance (H)
        double Lm = 0.011;          // Mutual inductance (H)
        double Np = 2.0;            // Number of pole pairs
        double Damping = 0.02;      // General damping coefficient
        double Inertia = 0.2;       // Rotational inertia (kg·m²)
        double Fd = 0.001;          // Viscous (dynamic) friction coeff
        double Fc = 0.001;          // Coulomb (sliding) friction torque [N*m]
        double Fs = 0.01;           // Static friction torque [N*m]
        double StribeckOmega = 2.0; // Stribeck scale [rad/s]
        double FricSmoothK = 50.0;  // Smoothing gain for tanh sign

        std::string toString() const;
    };

    // Audio filter types for post-processing
    enum class FilterType {
        PeakingEQ,
        HighPassFilter,
        LowPassFilter,
        NotchFilter
    };

    // Sound filter configuration
    struct SoundFilter {
        FilterType Type = FilterType::PeakingEQ;
        float Gain = 0.0f;
        float Frequency = 1000.0f;
        float Q = 1.0f;

        SoundFilter() = default;
        SoundFilter(FilterType filterType, float gain, float frequency, float q)
            : Type(filterType), Gain(gain), Frequency(frequency), Q(q) {}

        std::string toString() const;
    };

    // Harmonic amplitude envelope over frequency range
    struct HarmonicAmplitude {
        double Start = 0.0;
        double StartValue = 0.0;
        double End = 0.0;
        double EndValue = 0.0;
        double MinimumValue = 0.0;
        double MaximumValue = 0x60;

        std::string toString() const;
    };

    // Frequency range where harmonic is active
    struct HarmonicDataRange {
        double Start = 0.0;
        double End = -1.0;

        std::string toString() const;
    };

    // Individual harmonic component definition
    struct HarmonicData {
        double Harmonic = 0.0;                  // Harmonic frequency multiplier
        HarmonicAmplitude Amplitude{};          // Amplitude envelope
        HarmonicDataRange Range{};              // Active frequency range
        double Disappear = 0.0;                 // Disappear frequency (-1 = never)

        std::string toString() const;
    };

    // Main train audio configuration
    std::vector<HarmonicData> GearSound;     // Gear-related harmonics
    std::vector<HarmonicData> HarmonicSound; // Motor harmonics
    bool UseFilters = false;                 // Enable audio filters
    std::vector<SoundFilter> Filters;        // Filter chain
    bool UseConvolutionFilter = true;        // Enable impulse response convolution
    int ImpulseResponseSampleRate = 192000;  // IR sample rate
    std::vector<float> ImpulseResponse;      // Impulse response data
    MotorSpecification MotorSpec{};          // Motor physical parameters
    double MotorVolumeDb = -2.0;             // Motor sound volume (dB)
    double TotalVolumeDb = 0.0;              // Total output volume (dB)

    // Calculate gear harmonics from gear ratio
    void setCalculatedGearHarmonic(int Gear1, int Gear2);

    std::string toString() const;

    // Load default impulse response from resources
    void loadDefaultImpulseResponse();

    // Resample impulse response to target sample rate
    std::vector<float> getImpulseResponse(int targetSampleRate) const;
};

} // namespace VvvfSimulator::Data