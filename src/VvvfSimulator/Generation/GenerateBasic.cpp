#include "GenerateBasic.hpp"
// STL Includes
#include <cinttypes>
#include <cmath>
#include <sstream>

namespace VvvfSimulator::Generation
{
	namespace GenerateBasic
	{
		using NAMESPACE_VVVF::Struct::VvvfValues;
		using NAMESPACE_VVVF::Struct::WaveValues;
		using NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;

		using NAMESPACE_VVVF::Struct::PwmCalculateValues;

		namespace WaveForm
		{
			/// <summary>
			///  Calculates one cycle of UVW.
			/// </summary>
			/// <param name="Control"></param>
			/// <param name="Sound"></param>
			/// <param name="Division"> Recommend : 120000 , Brief : 2000 </param>
			/// <param name="Precise"> True for more precise calculation when Freq < 1 </param>
			/// <returns> One cycle of UVW </returns>
			QVector<WaveValues> getUVWCycle(VvvfValues control, const YamlVvvfSoundData& sound, double initialPhase, qsizetype division, bool precise)
			{
				PwmCalculateValues preCalculate = YamlVvvfWave::calculateYaml(control, sound);
				double _F = control.sinAngleFreq;
				if (_F < preCalculate.minimumFrequency && control.controlFrequency > 0) _F = preCalculate.minimumFrequency;

				if (precise)
				{
					double _K = (_F > 0.01 && _F < 1) ? 1 / _F : 1;
					count = static_cast<int64_t>(std::round(division * _K));
				}
				else
					count = division;
				double invDeltaT = count * _F;
				
				control.generationCurrentTime = 0;
				control.sinTime = 0;
				control.sawTime = 0;
				
				return getUVW(control, sound, initialPhase, static_cast<double>(invDeltaT), count);
			}

			/// <summary>
			/// Calculates WaveForm of UVW in 1 sec.
			/// </summary>
			/// <param name="Control"></param>
			/// <param name="Sound"></param>
			/// <param name="InitialPhase"></param>
			/// <param name="Division"> Recommend : 120000 , Brief : 2000 </param>
			/// <param name="Precise"> True for more precise calculation when Freq < 1</param>
			/// <returns> WaveForm of UVW in 1 sec.</returns>
			QVector<WaveValues> getUVWSec(VvvfValues control, const YamlVvvfSoundData& sound, double initialPhase, qsizetype division, bool precise)
			{
				PwmCalculateValues preCalculate = YamlVvvfWave::calculateYaml(control, sound);
				double _F = control.sinAngleFreq;
				if (_F < preCalculate.minimumFrequency && control.controlFrequency > 0) _F = preCalculate.minimumFrequency;

				if (precise)
				{
					double _K = (_F > 0.01 && _F < 1) ? 1 / _F : 1;
					count = static_cast<int64_t>(std::round(division * _K));
				}
				else
					count = division;
				
				control.generationCurrentTime = 0;
				control.sinTime = 0;
				control.sawTime = 0;
				
				return getUVW(control, sound, initialPhase, static_cast<double>(count), count);
			}

			QVector<WaveValues> getUVW(VvvfValues control, const YamlVvvfSoundData& sound, double initialPhase, double invDeltaT, int64_t count)
			{
				PwmCalculateValues calculated_Values = YamlVvvfWave::calculateYaml(control, sound);
				QVector<WaveValues> PWM_Array(count + 1);
				for (qsizetype i = 0; i < PWM_Array.size(); i++)
				{
					double parameterSet = i / invDeltaT;
					control.generationCurrentTime = parameterSet;
					control.sineTime = parameterSet;
					control.sawTime = parameterSet;
					PWM_Array[i] = Calculate::calculatePhases(control, calculated_Values, initialPhase);
				}
				return PWM_Array;
			}
		}
		
		namespace Fourier
		{
			double getFourier(const QVector<WaveValues>& UVW, qsizetype N)
			{
				double integral = 0;
				double dt = 1.0 / (UVW.size() - 1);

				for (qsizetype i = 0; i < UVW.size(); i++)
				{
					double iTime = InternalMath::M_2PI * i / (UVW.size() - 1);
					double sum = (UVW.at(i).U - UVW.at(i).V) * InternalMath::sine(N * iTime) * dt;
					integral += sum;
				}
				return integral;
			}

			double getFourierFast(const QVector<WaveValues>& UVW, qsizetype N)
			{
				double integral = 0;

				int64_t Ft = 0;
				double Time = 0;

				for (qsizetype i = 0; i < UVW.size(); i++)
				{
					auto iFt = static_cast<int64_t>(UVW.at(i).U - UVW.at(i).V);

					if (i == 0)
					{
						Ft = iFt;
						continue;
					}

					if (Ft == iFt) continue;
					double iTime = InternalMath::M_2PI * i / (UVW.size() - 1);
					double sum = (InternalMath::cosine(N * Time) - InternalMath::cosine(N * iTime)) * Ft / N;
					integral += sum;

					Time = iTime;
					Ft = iFt;
				}
				return integral / InternalMath::M_2PI;
			}
		}

		QVector<double> getFourierCoefficients(const QVector<WaveValues>& UVW, qsizetype N)
		{
			QVector<double> coefficients(N);
			for (qsizetype n = 1; n <= coefficients.size(); n++)
			{
				double result = getFourierFast(UVW, n);
				coefficients.at(n - 1) = result;
			}
			return coefficients;
		}

		/// <summary>
		/// Gets Fourier series coefficients
		/// </summary>
		/// <param name="Control">Make sure you put cloned data.</param>
		/// <param name="Sound"></param>
		/// <param name="Delta"></param>
		/// <param name="N"></param>
		/// <returns></returns>
		QVector<double> getFourierCoefficients(VvvfValues control, const YamlVvvfSoundData& sound, qsizetype delta, qsizetype N)
		{
			control.allowRandomFreqMove = false;
			auto PWM_Array = WaveForm::getUVWCycle(control, sound, InternalMath::M_PI_6, delta, false);
			return getFourierCoefficients(PWM_Array, N);
		}

		std::string getDesmosFourierCoefficientsArray(const QVector<double>& coefficients)
		{
			std::ostringstream array("C = [");
			for (auto i = coefficients.begin(); i < coefficients.end(); i++)
			{
				array << (i == coefficients.begin() ? "" : " ,") << *i;
			}
			array << ']';
			return array.str();
		}

		/// <summary>
		/// Do clone about control!
		/// </summary>
		/// <param name="Sound"></param>
		/// <param name="Control"></param>
		/// <returns></returns>
		double getVoltageRate(const VvvfValues& control, const YamlVvvfSoundData& sound, bool precise, bool fixSign = true)
		{
			auto PWM_Array = WaveForm::getUVWCycle(control, sound, InternalMath::M_PI_6, 120000, precise);
			return getVoltageRate(PWM_Array, fixSign);
		}
		double getVoltageRate(const QVector<WaveValues>& UVW, bool fixSign = true)
		{
			double result = getFourierFast(UVW, 1) / VoltageConvertFactor;
			if (fixSign) result = std::fabs(result);
			return result;
		}
	}
}
