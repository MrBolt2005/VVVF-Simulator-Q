#include "Struct.hpp"

// Standard Library
#include <algorithm>
#include <sstream>

namespace VvvfSimulator::Data
{
    void Vvvf::sortAcceleratePattern(bool inverse)
    {
        std::sort(acceleratePattern.begin(), acceleratePattern.end(),
                  [inverse](const PulseControl &a, const PulseControl &b)
                  {
                      return inverse
                                 ? (a.controlFrequencyFrom < b.controlFrequencyFrom)
                                 : (a.controlFrequencyFrom > b.controlFrequencyFrom);
                  });
    }

    void Vvvf::sortBrakingPattern(bool inverse)
    {
        std::sort(brakingPattern.begin(), brakingPattern.end(),
                  [inverse](const PulseControl &a, const PulseControl &b)
                  {
                      return inverse
                                 ? (a.controlFrequencyFrom < b.controlFrequencyFrom)
                                 : (a.controlFrequencyFrom > b.controlFrequencyFrom);
                  });
    }

    bool Vvvf::hasCustomPwm() const
    {
        auto hasCustom = [](const PulseControl &pc)
        {
            using PT = PulseControl::Pulse::PulseTypeName;
            return pc.pulseMode.pulseType == PT::CHM ||
                   pc.pulseMode.pulseType == PT::SHE;
        };

        return std::any_of(acceleratePattern.begin(), acceleratePattern.end(),
                           hasCustom) ||
               std::any_of(brakingPattern.begin(), brakingPattern.end(), hasCustom);
    }

    std::string Vvvf::toString() const
    {
        std::ostringstream oss;
        oss << "Vvvf { " << "Level: " << level
            << ", AccelPatterns: " << acceleratePattern.size()
            << ", BrakePatterns: " << brakingPattern.size()
            << ", HasCustomPwm: " << (hasCustomPwm() ? "true" : "false") << " }";
        return oss.str();
    }

    // ===== Analysis Methods (ex-Analyze) =====

    namespace {
        /// Helper to find control data in sorted pattern list
        template <typename TPattern>
        std::optional<TPattern>
        findCurrentPattern(const std::vector<TPattern> &patterns,
                                double controlFrequency)
        {
            if (patterns.empty())
                return std::nullopt;

            for (auto it = patterns.rbegin(); it != patterns.rend(); ++it)
            {
                if (controlFrequency >= it->controlFrequencyFrom)
                    return *it;
            }
            return patterns.front();
        }
    }

    Vvvf::ControlFrequencyData
    Vvvf::getControlFrequencyData(double controlFrequency) const
    {
        ControlFrequencyData data;
        auto accelPattern = getCurrentAcceleratePattern(controlFrequency);
        auto brakePattern = getCurrentBrakingPattern(controlFrequency);

        auto fillData = [&](const PulseControl& pattern) {
            // Map baseWave to waveForm
            data.waveForm = static_cast<WaveForm>(pattern.pulseMode.baseWave);
            
            // Get amplitude value based on mode
            if (pattern.amplitude.mode == PulseControl::AmplitudeValue::AmplitudeMode::Const) {
                data.amplitude = pattern.amplitude.constant;
            } else {
                data.amplitude = pattern.amplitude.constant; // TODO: Calculate moving value
            }
            
            // Get carrier frequency
            if (pattern.asyncModulationData.carrierWaveData.mode == 
                PulseControl::AsyncControl::CarrierFrequency::CarrierFrequencyMode::Const) {
                data.carrierFrequency = pattern.asyncModulationData.carrierWaveData.constant;
            } else {
                data.carrierFrequency = pattern.asyncModulationData.carrierWaveData.constant; // TODO: Calculate
            }
            
            // Get dipolar from pulse data or use default
            auto dipolarIt = pattern.pulseMode.pulseData.find(PulseControl::Pulse::PulseDataKey::Dipolar);
            if (dipolarIt != pattern.pulseMode.pulseData.end()) {
                data.dipolar = static_cast<int>(dipolarIt->second.constant);
            } else {
                data.dipolar = 3; // Default
            }
            
            // Map pulse type to pulse mode
            switch (pattern.pulseMode.pulseType) {
                case PulseControl::Pulse::PulseTypeName::ASYNC:
                    data.pulseMode = PulseMode::Async;
                    break;
                case PulseControl::Pulse::PulseTypeName::CHM:
                    data.pulseMode = PulseMode::CHM;
                    break;
                case PulseControl::Pulse::PulseTypeName::SHE:
                    data.pulseMode = PulseMode::SHE;
                    break;
                case PulseControl::Pulse::PulseTypeName::HO:
                    data.pulseMode = PulseMode::HO;
                    break;
                default:
                    data.pulseMode = PulseMode::PWM;
                    break;
            }
        };

        if (accelPattern) {
            fillData(*accelPattern);
        } else if (brakePattern) {
            fillData(*brakePattern);
        } else {
            data.waveForm = WaveForm::Sine;
            data.amplitude = 0.0;
            data.carrierFrequency = 0.0;
            data.dipolar = 3;
            data.pulseMode = PulseMode::PWM;
        }
        return data;
    }

    std::optional<Vvvf::AcceleratePattern>
    Vvvf::getCurrentAcceleratePattern(double controlFrequency) const
    {
        return findCurrentPattern(acceleratePattern, controlFrequency);
    }

    std::optional<Vvvf::BrakingPattern>
    Vvvf::getCurrentBrakingPattern(double controlFrequency) const
    {
        return findCurrentPattern(brakingPattern, controlFrequency);
    }

    double Vvvf::getFreeRunFrequency(double controlFrequency) const
    {
        // TODO: Implement free run frequency calculation when structure is clarified
        // For now, return the control frequency unchanged
        return controlFrequency;
    }

    double Vvvf::getMinimumFrequency() const
    {
        double minFreq = 0.0;
        if (!acceleratePattern.empty())
            minFreq = acceleratePattern.front().controlFrequencyFrom;
        if (!brakingPattern.empty())
        {
            double brakeMin = brakingPattern.front().controlFrequencyFrom;
            if (minFreq == 0.0 || brakeMin < minFreq)
                minFreq = brakeMin;
        }
        return minFreq;
    }

    double Vvvf::getMaximumFrequency() const
    {
        double maxFreq = 0.0;
        for (const auto &pattern : acceleratePattern)
            if (pattern.controlFrequencyFrom > maxFreq)
                maxFreq = pattern.controlFrequencyFrom;
        for (const auto &pattern : brakingPattern)
            if (pattern.controlFrequencyFrom > maxFreq)
                maxFreq = pattern.controlFrequencyFrom;
        return maxFreq;
    }

    bool Vvvf::requiresCustomPwm(double controlFrequency) const
    {
        auto data = getControlFrequencyData(controlFrequency);
        return data.pulseMode == PulseMode::CHM || data.pulseMode == PulseMode::SHE;
    }

    double Vvvf::getAmplitude(double controlFrequency) const
    {
        return getControlFrequencyData(controlFrequency).amplitude;
    }

    Vvvf::PulseMode Vvvf::getPulseMode(double controlFrequency) const
    {
        return getControlFrequencyData(controlFrequency).pulseMode;
    }

    double Vvvf::getCarrierFrequency(double controlFrequency) const
    {
        return getControlFrequencyData(controlFrequency).carrierFrequency;
    }

    bool Vvvf::isAsyncMode(double controlFrequency) const
    {
        auto accelPattern = getCurrentAcceleratePattern(controlFrequency);
        if (accelPattern && accelPattern->pulseMode.pulseType == PulseControl::Pulse::PulseTypeName::ASYNC)
            return true;
        auto brakePattern = getCurrentBrakingPattern(controlFrequency);
        return brakePattern && brakePattern->pulseMode.pulseType == PulseControl::Pulse::PulseTypeName::ASYNC;
    }

    int Vvvf::getDipolar(double controlFrequency) const
    {
        return getControlFrequencyData(controlFrequency).dipolar;
    }
}
