#pragma once

#include <array>

#include "../../Vvvf/Struct.hpp"

namespace VvvfSimulator::Generation::Motor::GenerateMotorCore
{
	using NAMESPACE_VVVF::Struct::WaveValues;

	// The following motor models are from
  //《Research on some key technologies of high performance frequency converter for asynchronous motor》Wang siran, Zhejiang University

	struct MotorSpecification
	{
		double R_s     = 1.898;    // stator resistance (ohm)
		double R_r     = 1.45;     // rotor resistance (ohm)
		double L_s     = 0.196;    // stator inductance (H)
		double L_r     = 0.196;    // rotor inductance (H)
		double L_m     = 0.187;    // mutual inductance (H)
		double NP      = 2;        // number of pole pairs
		double DAMPING = 500.0;    // damping coefficient
		double INERTIA = 0.05;     // rotational inertia mass (kg*m^2)
		double STATICF = 0.005879; // static friction coefficient (N*m*s)

		constexpr MotorSpecification() noexcept = default;
		constexpr MotorSpecification(double R_s, double R_r, double L_s, double L_r, double L_m, double NP, double DAMPING, double INERTIA, double STATICF) noexcept :
			R_s(R_s), R_r(R_r), L_s(L_s), L_r(L_r), L_m(L_m), NP(NP), DAMPING(DAMPING), INERTIA(INERTIA), STATICF(STATICF) {}
		constexpr MotorSpecification(const MotorSpecification& other) noexcept = default;
		constexpr MotorSpecification(MotorSpecification&& other) noexcept = default;
	};

	struct MotorParameter
	{
		// Inherent Parameters
		double rateCurrent = 16.5;        // rated current (A)
		std::array<double, 3> Iabc{};     // current in abc frame (A)
		std::array<double, 3> Idq0{};     // current in dq0 frame (A)
		std::array<double, 3> Uabc{};     // voltage in abc frame (V)
		std::array<double, 3> Udq0{};     // voltage in dq0 frame (V)
		double wsl = 0.0;                 // load speed (rad/s)
		double w_r = 0.0;                 // rotor speed (rad/s)

		// Response mechanical parameters
		double sita_r = 0.0;					    // rotor angle (rad)
		double w_mr   = 0.0;					    // rotor speed (rad/s)
		double sitamr = 0.0;					    // rotor angle (rad)
		double TL     = 1.0;					    // load torque (N*m)
		double Te     = 0.0;					    // electromagnetic torque (N*m)
		double r_Flux = 0.0;					    // rotor flux (Wb)

		// Others
		double i_m1 = 0.0;					      // current in phase 1 (A)
		std::array<double, 3> preIdq0{};  // previous Idq0
		std::array<double, 3> diffIdq0{}; // difference of Idq0
	};

	struct Motor
	{
		double samplingFrequency;
		MotorSpecification specification;

		constexpr Motor() = default;
		constexpr Motor(int samplingFrequency, const MotorSpecification& specification, const MotorParameter& parameter) :
			samplingFrequency(samplingFrequency), specification(specification), m_parameter(parameter) {}
		constexpr Motor(const Motor& other) = default;
		constexpr Motor(Motor&& other) = default;

		constexpr       MotorParameter       parameter() const noexcept { return m_parameter; }
		constexpr const MotorParameter& constParameter() const noexcept { return m_parameter; }
		void updateParameter(const WaveValues &voltage, double theta);

	private:
		MotorParameter m_parameter;
	};
}