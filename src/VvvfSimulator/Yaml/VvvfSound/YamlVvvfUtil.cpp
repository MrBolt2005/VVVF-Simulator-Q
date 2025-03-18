#include "YamlVvvfUtil.hpp"

// Standard Library
#include <exception>
#include <functional>
// Packages
#include <QMessageBox>
#include <QThreadPool>
#include <QVector>
// Internal
#include "../../Generation/GenerateBasic.hpp"
#include "../../Vvvf/Struct.hpp"

namespace NAMESPACE_YAMLVVVFSOUND::YamlVvvfUtil::AutoModulationIndexSolver
{
	bool SolveConfiguration::run()
	{
		if (soundData.AcceleratePattern.size() == 0 || soundData.BrakingPattern.size() == 0) return false;

		auto &accel = soundData.AcceleratePattern;
		auto &brake = soundData.BrakingPattern;

		for (const auto &controlData : accel)
			if (controlData.ControlFrequencyFrom < 0.0) return false;

		for (const auto &controlData : brake)
			if (controlData.ControlFrequencyFrom < 0.0) return false;
		
		soundData.sortAcceleratePattern(true);
		soundData.sortBrakingPattern(true);

		QThreadPool taskList;

		const auto solveModulationIndex = [](
			const double targetFrequency,
			InternalFunctionParameter &param,
			const std::function<void(double)> &testAmplitudeSetter,
			const std::function<void(double)> &amplitudeSetter
		)
		{
			try
			{
				double desireVoltageRate = std::min(targetFrequency / param.maxFrequency * param.maxVoltageRate, 1.0);

				NAMESPACE_VVVF::Struct::VvvfValues control;
				control.sinAngleFreq = targetFrequency * NAMESPACE_VVVF::InternalMath::m_2PI;
				control.controlFrequency = targetFrequency;
				control.masconOff = false;
				control.freeRun = false;
				control.brake = param.isBrakePattern;
				control.allowRandomFreqMove = false;

				const auto solveFunction = [&](double amplitude)
				{
					testAmplitudeSetter(amplitude);
					const double difference = VvvfSimulator::Generation::GenerateBasic::Fourier::getVoltageRate(control, param.configuration.soundData, true, false) - desireVoltageRate;
					return difference * 100.0;
				};
				double properAmplitude;
				switch (param.configuration.solverType)
				{
				case EquationSolverType::Bissection:
					properAmplitude = NAMESPACE_VVVF::InternalMath::EquationSolver::BisectionMethod(solveFunction).calculate(-10.0, 10.0, param.configuration.precision, param.configuration.maxEffort);
					break;
				case EquationSolverType::Newton:
					properAmplitude = NAMESPACE_VVVF::InternalMath::EquationSolver::NewtonMethod(solveFunction, 0.05).calculate(desireVoltageRate, param.configuration.precision, param.configuration.maxEffort);
					break;
				default: properAmplitude = 0.0;
				}
				amplitudeSetter(properAmplitude);
			}
			catch (const std::exception &ex)
			{
				// Put code here later!
			};
		};

		const auto autoModulationIndexTask = [&](QThreadPool &taskList, InternalFunctionParameter &functionParameter)
		{
			auto &ysd = functionParameter.isBrakePattern ?
				functionParameter.configuration.soundData.BrakingPattern :
				functionParameter.configuration.soundData.AcceleratePattern;
			auto &parameter = ysd.at(functionParameter.index).Amplitude.Default;
			auto &parameterPowerOn = ysd.at(functionParameter.index).Amplitude.PowerOn;
			auto &parameterPowerOff = ysd.at(functionParameter.index).Amplitude.PowerOff;

			parameter.DisableRangeLimit = false;
			parameter.MaxAmplitude = -1.0;
			parameter.CutOffAmplitude = 0.0;
			parameterPowerOn.DisableRangeLimit = false;
			parameterPowerOn.MaxAmplitude = -1.0;
			parameterPowerOn.CutOffAmplitude = 0.0;
			parameterPowerOff.DisableRangeLimit = false;
			parameterPowerOff.MaxAmplitude = -1.0;
			parameterPowerOff.CutOffAmplitude = 0.0;
			parameter.StartFrequency =
				ysd.at(functionParameter.index).ControlFrequencyFrom <= 0.0 ?
				0.01 :
				ysd.at(functionParameter.index).ControlFrequencyFrom;
			parameterPowerOn.StartFrequency = parameter.StartFrequency;
			parameterPowerOff.StartFrequency = parameter.StartFrequency;
			parameter.EndFrequency = functionParameter.index + 1 >= ysd.size() ?
				functionParameter.maxFrequency + (ysd.at(functionParameter.index).ControlFrequencyFrom == functionParameter.maxFrequency ? 0.1 : 0) :
				ysd.at(functionParameter.index + 1).ControlFrequencyFrom - 0.1;
			parameterPowerOn.EndFrequency = parameter.EndFrequency;
			parameterPowerOff.EndFrequency = parameter.EndFrequency;

			if(parameter.Mode == YamlVvvfSoundData::YamlControlData::YamlAmplitude::AmplitudeParameter::AmplitudeMode::Table)
			{
				std::vector<YamlVvvfSoundData::YamlControlData::YamlAmplitude::AmplitudeParameter::AmplitudeTableEntry> modulationIndexList;
				parameter.StartAmplitude = 0.0;
				if (functionParameter.configuration.isTableDivisionPerHz)
				{
					for (double _Freq = parameter.StartFrequency; _Freq <= parameter.EndFrequency; _Freq += 1.0 / functionParameter.configuration.tableDivision)
					{
						modulationIndexList.push_back({_Freq, 0.0});
					}
					modulationIndexList.push_back({parameter.EndFrequency, 0.0});
				}
				else
					for (int i = 0; i <= functionParameter.configuration.tableDivision; i++)
						modulationIndexList.push_back({(parameter.EndFrequency - parameter.StartFrequency) / functionParameter.configuration.tableDivision * i + parameter.StartFrequency, 0.0});
				
				for (size_t index = 0; index < modulationIndexList.size(); index++)
				{
					double &frequency = modulationIndexList.at(index).Frequency;
					parameter.AmplitudeTable = modulationIndexList;
					auto tester = functionParameter; // Clone
					taskList.start([&]()
					{
						solveModulationIndex(frequency, tester, [&](double amplitude)
						{
						YamlVvvfSoundData::YamlControlData &target =
							(tester.isBrakePattern ?
							tester.configuration.soundData.BrakingPattern :
							tester.configuration.soundData.AcceleratePattern).at(tester.index);
							target.Amplitude.Default.AmplitudeTable = { {frequency, amplitude} };
						},
						[&](double amplitude)
						{
							modulationIndexList.at(index) = {frequency, amplitude};
							parameter.AmplitudeTable = modulationIndexList;
						});
					});
				}
			}
		};
		for (size_t i = 0; i < accel.size(); i++)
		{
			InternalFunctionParameter param
			{
				*this,
				false,
				i,
				accelEndFrequency,
				accelMaxVoltage / 100.0
			};
			autoModulationIndexTask(taskList, param);
		}
		for (size_t i = 0; i < brake.size(); i++)
		{
			InternalFunctionParameter param
			{
				*this,
				true,
				i,
				accelEndFrequency,
				accelMaxVoltage / 100.0
			};
			autoModulationIndexTask(taskList, param);
		}
		taskList.waitForDone();

		soundData.sortAcceleratePattern(false);
		soundData.sortBrakingPattern(false);

		return true;
	}
}