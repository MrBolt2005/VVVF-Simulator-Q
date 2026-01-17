#include "Modulation.hpp"
// Standard Library
#include <cmath>
#include <exception>
#include <random>
#include <string>
// Internal
#include "InternalMath.hpp"

namespace VvvfSimulator::Vvvf::Modulation {
    bool DeltaSigma::process(double input, double nowTime) noexcept
    {
        double dt = nowTime - lastProcessTime;
        lastProcessTime = nowTime;

        double quantized = lastOutBit ? 1.0 : -1.0;
        integrator += (input - quantized) * dt;

        if (nowTime - lastUpdateTime >= feedbackInterval) {
            lastOutBit = integrator >= 0.0;
            lastUpdateTime = nowTime;
        }

        return lastOutBit;
    }

    void DeltaSigma::reset(double nowTime) noexcept
    {
        integrator = 0.0;
        lastOutBit = 0;
        lastUpdateTime = nowTime;
        lastProcessTime = nowTime;
    }

    void DeltaSigma::resetIfLastTime(double lastTime) noexcept
    {
        if (lastTime != lastProcessTime)
            reset(lastTime);
    }

    RandomFrequency::RandomFrequency(int64_t seed)
        : randomInstance(seed)
    {}

    using ParameterT = Model::ElectricalParameter::CarrierParameter::RandomFrequency;
    
    RandomFrequency::RandomFrequency(const RandomFrequency &other) = default;

    RandomFrequency::RandomFrequency(const decltype(parameter) &param, double baseFrequency)
        : parameter(param)
        , randomInstance(parameter ? parameter->seed : 0)
        , baseFrequency(baseFrequency)
    {}

    void RandomFrequency::setState(bool simple, double time)
    {
        this->simple = simple;
        this->time = time;
    }

    void RandomFrequency::setCustomParameter(const decltype(parameter) &param, double baseFrequency)
    {
        this->parameter = parameter;
        this->baseFrequency = baseFrequency;
    }

    double RandomFrequency::calculate()
    {
        if (!parameter) return NAN;
        if (simple) return baseFrequency;

        if (lastUpdateTime + parameter->interval < time) {
            std::uniform_real_distribution urd(-0.5, 0.5);
            lastRange = urd(randomInstance) * parameter->range;
            lastUpdateTime = time;
        }
        return baseFrequency + lastRange;
    }

    void RandomFrequency::resetTime(double time)
    {
        this->time = time;
        lastUpdateTime = time;
    }

    PeriodicFrequency::PeriodicFrequency() = default;
    PeriodicFrequency::PeriodicFrequency(const PeriodicFrequency &other) = default;
    
    void PeriodicFrequency::setState(bool simple, double time)
    {
        this->simple = simple;
        this->time = time;
    }

    void PeriodicFrequency::setCustomParameter(const decltype(parameter) &param, const decltype(baseWaveType) baseWaveType)
    {
        this->parameter = parameter;
        this->baseWaveType = baseWaveType;
    }

    double PeriodicFrequency::calculate()
    {
        if (!parameter || !baseWaveType) return NAN;
        if (simple) return (parameter->highest + parameter->lowest) * 0.5;

        if (lastInterval != parameter->interval) {
            lastTime = time - (time - lastTime) * (lastInterval == 0.0 ? 1.0 : parameter->interval / lastInterval);
            lastInterval = parameter->interval;
            lastTime = time;
        }

        if (time - lastTime >= parameter->interval)
            lastTime = time;
        
        using namespace InternalMath;
        double phase = parameter->interval > 0.0 ? (time - lastTime) / parameter->interval * m_2PI : 0.0;
        double retVal = (parameter->highest - parameter->lowest) * 0.5;
        switch (baseWaveType) {
        case BaseWaveT::Sine:
            retVal *= (Functions::sine(phase) + 1.0);
            break;
        case BaseWaveT::Triangle:
            retVal *= (Functions::triangle(phase) + 1.0);
            break;
        case BaseWaveT::Square:
            retVal *= (Functions::square(phase) + 1.0);
            break;
        case BaseWaveT::SawUp:
            retVal *= (Functions::saw(phase) + 1.0);
            break;
        default: // case BaseWaveT::SawDown:
            retVal *= (1.0 - Functions::saw(phase));
            break;
        }
        retVal += parameter->lowest;
        return retVal;
    }

    void PeriodicFrequency::resetTime(double time)
    {
        this->time = time;
        lastTime = time;
    }

    double Carrier::frequency() const noexcept
    {
        using namespace InternalMath;
        return angleFrequency / m_2PI;
    }

    void Carrier::setFrequency(double value) noexcept
    {
        using namespace InternalMath;
        angleFrequency = m_2PI * value;
    }

    void Carrier::setAsyncAngleFrequency(double value) noexcept
    {
        if (value == 0.0)
            time = 0.0;
        else
            time = angleFrequency / value * time;
        angleFrequency = value;
    }

    void Carrier::setAsyncFrequency(double value) noexcept
    {
        if (value == 0.0)
            time = 0.0;
        else
            time = frequency / value * time;
        frequency = value;
    }

    double Carrier::phase() const noexcept
    {
        return time * angleFrequency;
    }

    Carrier::Carrier() = default;
    Carrier::Carrier(const Carrier &other) = default;

    double Carrier::calculateBaseCarrierFrequency(double time, const Model::ElectricalParameter &electricalState)
    {
        if (electricalState.isNone)
            return 0.0;
        
        if (!electricalState.carrierFrequency) {
            static constexpr std::string_view exWhat = Q_FUNC_INFO + ": electricalState: Model::ElectricalParameter::carrierFrequency is null.";
            throw std::invalid_argument(exWhat);
        }
        
        auto &baseCarrierFreqParameter = electricalState.carrierFrequency->baseFrequency;
        double baseCarrierFrequency = 0.0;

        if (auto constant = std::get_if<std::shared_ptr<
            Model::ElectricalParameter::CarrierParameter::ConstantFrequency>>(&baseCarrierFreqParameter)) {
            if (constant->get() == nullptr) {
                static constexpr std::string_view exWhat = Q_FUNC_INFO
                    + ": electricalState: Model::ElectricalParameter::carrierFrequency->baseFrequency is null.";
                throw std::invalid_argument(exWhat);
            }
            baseCarrierFrequency = *(constant->get());
        } else if (auto periodic = std::get_if<std::shared_ptr<
            Model::ElectricalParameter::CarrierParameter::PeriodicFrequency>>(&baseCarrierFreqParameter)) {
            if (periodic->get() == nullptr) {
                static constexpr std::string_view exWhat = Q_FUNC_INFO
                    + ": electricalState: Model::ElectricalParameter::carrierFrequency->baseFrequency is null.";
                throw std::invalid_argument(exWhat);
            }
            if (!electricalState.pulsePattern) {
                static constexpr std::string_view exWhat = Q_FUNC_INFO
                    + ": electricalState: Model::ElectricalParameter::pulsePattern is null.";
                throw std::invalid_argument(exWhat);
            }
            periodicInstance.setState(useSimpleFrequency, time);
            periodicInstance.setCustomParameter(
                *periodic, electricalState.pulsePattern->asyncModulationData
                               .carrierWaveData.vibrato.baseWave);
            baseCarrierFrequency = periodicInstance.calculate();
        }
        return baseCarrierFrequency;
    }

    double Carrier::calculateCarrierFrequency(double time, const Model::ElectricalParameter &electricalState)
    {
        if (electricalState.isNone)
            return 0.0;
        if (!electricalState.carrierFrequency) {
            static constexpr std::string_view exWhat = Q_FUNC_INFO
                + ": electricalState: Model::ElectricalParameter::carrierFrequency is null.";
            throw std::invalid_argument(exWhat);
        }
        
        double finalCarrierFreq, baseCarrierFreq = calculateBaseCarrierFrequency(time, electricalState);

        randomInstance.setState(useSimpleFrequency, time);
        randomInstance.setCustomParameter(
            electricalState.carrierFrequency->randomRange, baseCarrierFreq);
        
        finalCarrierFreq = randomInstance.calculate();
        return finalCarrierFreq;
    }

    void Carrier::processCarrierFrequency(double time, const Model::ElectricalParameter &electricalState)
    {
        return setAsyncFrequency(calculateCarrierFrequency(time, electricalState));
    }

    void Carrier::resetIFrequencyTime(double time) noexcept
    {
        randomInstance.resetTime(time);
        periodicInstance.resetTime(time);
    }
}