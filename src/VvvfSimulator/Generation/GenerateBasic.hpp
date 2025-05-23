// Internal Includes
#include "../Vvvf/Struct.hpp"
#include "../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"
// Package Includes
#include <QVector>

namespace VvvfSimulator::Generation
{
	namespace GenerateBasic
	{
		using NAMESPACE_VVVF::Struct::VvvfValues;
		using NAMESPACE_VVVF::Struct::WaveValues;
		using NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;

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
			QVector<WaveValues> getUVWCycle(VvvfValues control, const YamlVvvfSoundData& sound, double initialPhase, qsizetype division, bool precise);
			
			/// <summary>
			/// Calculates WaveForm of UVW in 1 sec.
			/// </summary>
			/// <param name="Control"></param>
			/// <param name="Sound"></param>
			/// <param name="InitialPhase"></param>
			/// <param name="Division"> Recommend : 120000 , Brief : 2000 </param>
			/// <param name="Precise"> True for more precise calculation when Freq < 1</param>
			/// <returns> WaveForm of UVW in 1 sec.</returns>
			QVector<WaveValues> getUVWSec(VvvfValues control, const YamlVvvfSoundData& sound, double initialPhase, qsizetype division, bool precise);

			QVector<WaveValues> getUVW(VvvfValues control, const YamlVvvfSoundData& sound, double initialPhase, double invDeltaT, qsizetype count);
		}
		
		namespace Fourier
		{
			constexpr double VoltageConvertFactor = 1.102657791;
			double getFourier(const QVector<WaveValues>& UVW, qsizetype N);
			
			double getFourierFast(const QVector<WaveValues>& UVW, qsizetype N);
			
			QVector<double> getFourierCoefficients(const QVector<WaveValues>& UVW, qsizetype N);
			
			/// <summary>
			/// Gets Fourier series coefficients
			/// </summary>
			/// <param name="Control">Make sure you put cloned data.</param>
			/// <param name="Sound"></param>
			/// <param name="Delta"></param>
			/// <param name="N"></param>
			/// <returns></returns>
			QVector<double> getFourierCoefficients(VvvfValues control, const YamlVvvfSoundData& sound, qsizetype delta, qsizetype N);

			std::string getDesmosFourierCoefficientsArray(const QVector<double>& coefficients);

			/// <summary>
			/// Do clone about control!
			/// </summary>
			/// <param name="Sound"></param>
			/// <param name="Control"></param>
			/// <returns></returns>
			double getVoltageRate(const VvvfValues& control, const YamlVvvfSoundData& sound, bool precise, bool fixSign = true);
			double getVoltageRate(const QVector<WaveValues>& UVW, bool fixSign = true);
		}
	}
}
