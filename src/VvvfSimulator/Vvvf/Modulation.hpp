#pragma once

// Copyright © 2025 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Vvvf/Modulation.hpp

// Standard Library
#include <memory>
// Packages
#include <QPointF>
// Internal
#include "InternalMath.hpp"
#include "Model.hpp"
#include "../Random/xoshiro256.hpp"

namespace VvvfSimulator::Vvvf::Modulation {
    class DeltaSigma {
        double integrator = 0.0;
        double lastProcessTime = 0.0;
        double lastUpdateTime = 0.0;
        bool lastOutBit = 0;

    public:
        double feedbackInterval = 1e-4;

        constexpr DeltaSigma() = default;
        constexpr DeltaSigma(const DeltaSigma &other) = default;

        bool process(double input, double nowTime) noexcept;
        void reset(double nowTime = 0.0) noexcept;
        void resetIfLastTime(double lastTime) noexcept;
    };

    struct IFrequency {
        virtual void setState(bool simple, double time) = 0;
        virtual double calculate() = 0;
        virtual void resetTime(double time) = 0;
    };

    class RandomFrequency : public IFrequency {
        using ParameterT = Model::ElectricalParameter::CarrierParameter::RandomFrequency;

        double time = 0.0;
        std::shared_ptr<ParameterT> parameter;
        bool simple = false;
        double baseFrequency = 0.0;

        double lastRange = 0.0;
        double lastUpdateTime = 0.0;
        Random::xoshiro256ss randomInstance;

    public:
        RandomFrequency(int64_t seed = 0);
        RandomFrequency(const RandomFrequency &other);
        RandomFrequency(const decltype(parameter) &param, double baseFrequency);

        void setState(bool simple, double time) override;
        void setCustomParameter(const decltype(parameter) &param, double baseFrequency);
        double calculate() override;
        void resetTime(double time) override;
    };

    class PeriodicFrequency : public IFrequency {
        using ParameterT = Model::ElectricalParameter::CarrierParameter::PeriodicFrequency;
        using BaseWaveT = Data::Vvvf::PulseControl::AsyncControl::CarrierFrequency::VibratoValue::BaseWaveType;

        bool simple = false;
        double time = 0.0;

        std::shared_ptr<ParameterT> parameter;
        std::optional<BaseWaveT> baseWaveType;
        double baseFrequency = 0.0;

        double lastInterval = 0.0;
        double lastTime = 0.0;

    public:
        PeriodicFrequency();
        PeriodicFrequency(const PeriodicFrequency &other);

        void setState(bool simple, double time) override;
        void setCustomParameter(const decltype(parameter) &param, const decltype(baseWaveType) baseWaveType);
        double calculate() override;
        void resetTime(double time) override;
    };

    class Carrier {
        RandomFrequency randomInstance;
        PeriodicFrequency periodicInstance;

    public:
        double angleFrequency = 0.0;
        double frequency() const noexcept;
        void setFrequency(double value) noexcept;

        double time = 0.0;
        void setAsyncAngleFrequency(double value) noexcept;
        void setAsyncFrequency(double value) noexcept;

        double phase() const noexcept;

        bool useSimpleFrequency = false;

        Carrier();
        Carrier(const Carrier &other);

        // @throws std::invalid_argument
        double calculateBaseCarrierFrequency(
            double time, const Model::ElectricalParameter &electricalState);
        // @throws std::invalid_argument
        double calculateCarrierFrequency(
            double time, const Model::ElectricalParameter &electricalState);
        // @throws std::invalid_argument
        void processCarrierFrequency(
            double time, const Model::ElectricalParameter &electricalState);
        void resetIFrequencyTime(double time) noexcept;
    };

    typedef std::array<double, 3> Vec3Base;
    class Vec3 : public Vec3Base {
    protected:
        constexpr double &x = (*this)[0];
        constexpr double &y = (*this)[1];
        constexpr double &z = (*this)[2];

    public:
        constexpr Vec3() noexcept = default;
        explicit constexpr Vec3(double x, double y, double z) noexcept
            : Vec3Base({x, y, z})
        {}
        constexpr Vec3(const Vec3 &other) noexcept = default;

        constexpr Vec3 operator+(double d) const noexcept {
            return Vec3(x + d, y + d, z + d);
        }

        constexpr Vec3 operator*(double d) const noexcept {
            return Vec3(x * d, y * d, z * d);
        }

        constexpr Vec3 operator-(double d) const noexcept {
            return Vec3(x - d, y - d, z - d);
        }

        constexpr Vec3 operator-(const Vec3 &other) const noexcept {
            return Vec3(x - other.x, y - other.y, z - other.z);
        }
    };

    class Vabc;
    class FunctionTime : public Vec3 {
    public:
        constexpr double &t0 = (*this)[0];
        constexpr double &t1 = (*this)[1];
        constexpr double &t2 = (*this)[2];

        constexpr Vabc getVabc(int sector) const noexcept;
    };

    class Valbe : public QPointF {
    public:
        constexpr double &alpha = rx();
        constexpr double &beta = ry();

        constexpr FunctionTime getFunctionTime(int sector) const noexcept {
            double t1{}, t2{};
            using namespace InternalMath;
            switch (sector) {
            case 1:
                t1 = m_SQRT3_2 * alpha - 0.5 * beta;
                t2 = beta;
            break;
            case 2:
                t1 = m_SQRT3_2 * alpha + 0.5 * beta;
                t2 = 0.5 * beta - m_SQRT3_2 * alpha;
            break;
            case 3:
                t1 = beta;
                t2 = -(m_SQRT3_2 * alpha + 0.5 * beta);
            break;
            case 4:
                t1 = 0.5 * beta - m_SQRT3_2 * alpha;
                t2 = -beta;
            case 5:
                t1 = -(m_SQRT3_2 * alpha + 0.5 * beta);
                t2 = m_SQRT3_2 * alpha - 0.5 * beta;
            break;
            case 6:
                t1 = -beta;
                t2 = m_SQRT3_2 * alpha + 0.5 * beta;
            // break;
            }
            return FunctionTime(t1, t2);
        }

        constexpr int estimateSector() const noexcept {
            int a = beta > 0.0 ? 0 : 1;
            int b = beta - m_SQRT3 * alpha > 0.0 ? 0 : 1;
            int c = beta + m_SQRT3 * alpha > 0.0 ? 0 : 1;
            switch (4 * a + 2 * b + c) {
            case 1:
                return 3;
            break;
            case 2:
                return 1;
            break;
            case 3:
            case 4:
                return 0;
            break;
            case 5:
                return 4;
            break;
            case 6:
                return 6;
            break;
            case 7:
                return 5:
            break;
            default: // + case 0:
                return 2;
            }
        }
    };

    class Vabc : public Vec3 {
    public:
        constexpr double &u = (*this)[0];
        constexpr double &v = (*this)[1];
        constexpr double &w = (*this)[2];

        constexpr Valbe clark() const noexcept {
            return Vabc(
                (2.0 * u - v - w) * double(1.0 / 3.0),
                (v - w) * double(1.0 / InternalMath::m_SQRT3)
            );
        }
    };

    constexpr Vabc FunctionTime::getVabc(int sector) const noexcept
    {
        Vabc Res;
        switch (sector)
        {
        case 1:
        {
            Res.u = T1 + T2 + 0.5 * T0;
            Res.v = T2 + 0.5 * T0;
            Res.w = 0.5 * T0;
        }
        break;
        case 2:
        {
            Res.u = T1 + 0.5 * T0;
            Res.v = T1 + T2 + 0.5 * T0;
            Res.w = 0.5 * T0;
        }
        break;
        case 3:
        {
            Res.u = 0.5 * T0;
            Res.v = T1 + T2 + 0.5 * T0;
            Res.w = T2 + 0.5 * T0;
        }
        break;
        case 4:
        {
            Res.u = 0.5 * T0;
            Res.v = T1 + 0.5 * T0;
            Res.w = T1 + T2 + 0.5 * T0;
        }
        break;
        case 5:
        {
            Res.u = T2 + 0.5 * T0;
            Res.v = 0.5 * T0;
            Res.w = T1 + T2 + 0.5 * T0;
        }
        break;
        case 6:
        {
            Res.u = T1 + T2 + 0.5 * T0;
            Res.v = 0.5 * T0;
            Res.w = T1 + 0.5 * T0;
        }
        break;
        }
        return Res;
    }
}