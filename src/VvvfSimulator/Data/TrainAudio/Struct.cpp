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

// Data/TrainAudio/Struct.cpp
// Version 1.10.0.0

#include "Struct.hpp"
#include <sstream>
#include <cmath>

namespace VvvfSimulator::Data::TrainAudio {

std::string MotorSpecification::toString() const {
    std::ostringstream oss;
    oss << "MotorSpec{V=" << V 
        << ", Rs=" << Rs << ", Rr=" << Rr
        << ", Ls=" << Ls << ", Lr=" << Lr << ", Lm=" << Lm
        << ", Np=" << Np << ", Damping=" << Damping
        << ", Inertia=" << Inertia << "}";
    return oss.str();
}

std::string SoundFilter::toString() const {
    std::ostringstream oss;
    oss << "SoundFilter{Type=" << static_cast<int>(Type)
        << ", Gain=" << Gain << ", Freq=" << Frequency 
        << ", Q=" << Q << "}";
    return oss.str();
}

std::string HarmonicAmplitude::toString() const {
    std::ostringstream oss;
    oss << "Amplitude{" << Start << "->" << End 
        << ", " << StartValue << "->" << EndValue << "}";
    return oss.str();
}

std::string HarmonicDataRange::toString() const {
    std::ostringstream oss;
    oss << "Range{" << Start << "->" << End << "}";
    return oss.str();
}

std::string HarmonicData::toString() const {
    std::ostringstream oss;
    oss << "Harmonic{" << Harmonic << ", " 
        << Amplitude.toString() << ", " 
        << Range.toString() << "}";
    return oss.str();
}

void Struct::setCalculatedGearHarmonic(int Gear1, int Gear2) {
    std::vector<HarmonicData> gearHarmonicsList;
    
    // Calculate rotation frequency
    double Rotation = 120.0 / std::pow(2.0, MotorSpec.Np) / 60.0;

    // Define harmonic series with gear ratio factors
    std::vector<double> Harmonic = {
        9.0 * 2.0 * Gear1 / Gear2 * 189.0 / 225.0,
        9.0 * 2.0 * Gear1 / Gear2,
        9.0,
        1.0
    };

    for (size_t i = 0; i < Harmonic.size(); i++) {
        HarmonicAmplitude amplitude;
        amplitude.Start = 0.0;
        amplitude.StartValue = 0.0;
        amplitude.End = 40.0;
        amplitude.EndValue = 0.1 * std::pow(1.4, -static_cast<double>(i));
        amplitude.MinimumValue = 0.0;
        amplitude.MaximumValue = 0.1;

        HarmonicData data;
        data.Harmonic = Rotation * Gear1 * Harmonic[i];
        data.Amplitude = amplitude;
        data.Disappear = -1.0;

        gearHarmonicsList.push_back(data);
    }

    GearSound = gearHarmonicsList;
}

std::string Struct::toString() const {
    std::ostringstream oss;
    oss << "TrainAudioStruct{GearSound=" << GearSound.size()
        << ", HarmonicSound=" << HarmonicSound.size()
        << ", Filters=" << Filters.size()
        << ", MotorVolume=" << MotorVolumeDb << "dB"
        << ", TotalVolume=" << TotalVolumeDb << "dB}";
    return oss.str();
}

void Struct::loadDefaultImpulseResponse() {
    // TODO: Load default impulse response from Qt resources
    // This will be implemented when Qt resource system is integrated
    // For now, initialize with empty response
    ImpulseResponse.clear();
    ImpulseResponseSampleRate = 192000;
}

std::vector<float> Struct::getImpulseResponse(int targetSampleRate) const {
    if (ImpulseResponseSampleRate == targetSampleRate) {
        return ImpulseResponse;
    }

    // TODO: Implement resampling when audio generation is integrated
    // For now, return the original response
    return ImpulseResponse;
}

} // namespace VvvfSimulator::Data::TrainAudio
