#pragma once

// Copyright © 2025 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Vvvf/Model.hpp

// Standard Library
#include <array>
#include <memory>
#include <optional>
#include <variant>
// Packages
#include <QHash>
#include <QList>
// Internal
#include "Modulation.hpp"
#include "../Data/VehicleAudio.hpp"
#include "../Data/Vvvf.hpp"

namespace VvvfSimulator::Vvvf {
  // TODO: Replace by actual DeltaSigma header include
  namespace Modulation { struct DeltaSigma; } 
struct Model {
  Model() = default;

#pragma region ControlParameter
  double controlFrequency = 0.0;
  double freeFrequencyChange = 0.0;
  bool brake = false;
  bool freeRun = false;
  bool powerOff = false;
  bool allowBaseWaveTimeChange = true;

  void processControlParameter(double deltaTime,
                               const Data::Vvvf::JerkSettings &jerkSetting);

#pragma endregion

#pragma region MathematicalParameter
  double baseWaveAngleFreq = 0.0;
  double baseWaveTime = 0.0;
  double lastTime = 0.0;
  double time = 0.0;

  void setTimeAll(double time) noexcept;
  void addTimeAll(double time) noexcept;
  void resetTimeAll() noexcept;
  double deltaTime() const noexcept;

#pragma endregion

  class PhaseState;

#pragma region MotorParameter
  struct Motor {
    struct Status {
      // Inherent Parameters
      // --- Electrical dynamic states ---
      std::array<double, 3> Idq0, Vdq0;
      double Ωsl = 0.0;         // Slip angular frequency
      double Ωr = 0.0;          // Rotor electrical angular speed
      std::array<double, 2> Ir; // Rotor dq currents
      double Ωe = 0.0;          // Stator electrical angular speed

      // --- Mechanical dynamic states ---
      double Θr = 0.0;  // Electrical angle of rotor
      double Θmr = 0.0; // Mechanical rotor position (rad)
      double TL = 0.0;  // Load torque (N·m)
      double Te = 0.0;  // Electromagnetic torque (N·m)
      double Φr = 0.0;  // Rotor flux linkage magnitude (Wb)

      // --- Optional additional dynamic states (recommended additions) ---
      std::array<double, 2> flux_s; // Stator flux dq components
      std::array<double, 2> flux_r; // Rotor flux dq components

      // --- Diagnostic / monitoring ---
      double preTe = 0.0;
      double diffTe = 0.0;
      std::array<double, 3> preIdq0;
      std::array<double, 3> diffIdq0;

      constexpr Status() = default;
      constexpr Status(const Status &other) = default;

      // Debug: size of struct, in bytes
      constexpr auto sizeOf() const noexcept { return sizeof(*this); }
    };

    using MotorSpecification = Data::VehicleAudio::MotorSpecification;

    MotorSpecification specification;
    Status parameter;

    constexpr Motor() = default;
    constexpr Motor(const Motor &other) = default;
    constexpr Motor(const MotorSpecification &spec) : specification(spec) {}

    void process(double dt, double ωe, PhaseState voltage);

    constexpr auto sizeOf() const noexcept { return sizeof(*this); }
  };
  Motor motor;
#pragma endregion

#pragma region ElectricParameter
  struct ElectricalParameter {
    struct CarrierParameter {
      struct RandomFrequency {
        double range;
        double interval;
        int64_t seed;

        constexpr RandomFrequency() = default;
        constexpr RanfomFrequency(double range, double interval, int64_t seed)
            : range(range), interval(interval), seed(seed) {}
      };
      typedef double ConstantFrequency;
      struct PeriodicFrequency {
        double highest, lowest, interval;

        constexpr PeriodicFrequency() = default;
        constexpr PeriodicFrequency(double hi, double lo, double in)
            : highest(hi), lowest(lo), interval(in) {}
      };

      std::shared_ptr<RandomFrequency> randomRange;
      std::variant<std::shared_ptr<ConstantFrequency>,
        std::shared_ptr<PeriodicFrequency>> baseFrequency;

      constexpr CarrierParameter() = default;
      constexpr CarrierParameter(const CarrierParameter &other) = default;

      constexpr auto sizeOf() const noexcept { return sizeof(*this); }
    };

    bool isNone = true;
    bool isZeroOutput = true;
    int pwmLevel;
    // TODO: Determine if/whether it would be better to leave these as shared
    // pointers or as optionals
    std::optional<Data::Vvvf::PulseControl> pulsePattern;
    std::optional<CarrierParameter> carrierFrequency;
    std::optional<QHash<Data::Vvvf::PulseControl::Pulse::PulseDataKey, double>>
        pulseData;
    double baseWaveFrequency;
    double baseWaveAngleFrequency;
    std::optional<double> baseWaveAmplitude;

    ElectricalParameter();
    ElectricalParameter(
      bool isNone,
      bool isZeroOutput,
      int pwmLevel,
      const Data::Vvvf::PulseControl *pulsePattern,
      const CarrierParameter *carrierFrequency,
      const QHash<Data::Vvvf::PulseControl::Pulse::PulseDataKey,
        double> *pulseData,
      double baseWaveFrequency,
      const double *baseWaveAmplitude
    );
    ElectricalParameter(int pwmLevel, double baseWaveFrequency);

    constexpr static auto dummy = sizeof(pulseData);
    constexpr auto sizeOf() const noexcept { return sizeof(*this); }
  };

  ElectricalParameter electricalState = ElectricalParameter(2, 0.0);
#pragma endregion

#pragma region Modulation
  std::array<Modulation::DeltaSigma, 3> deltaSigmaInstances;
  void resetDeltaSigmaInstance();
  Modulation::Carrier carrierInstance;
#pragma endregion

  constexpr auto sizeOf() const noexcept { return sizeof(*this); }
};
} // namespace VvvfSimulator::Vvvf