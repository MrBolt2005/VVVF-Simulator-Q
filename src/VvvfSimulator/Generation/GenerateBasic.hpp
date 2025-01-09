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
			QVector<WaveValues_UVW> GetUVWCycle(VvvfValues& Control, YamlVvvfSoundData& Sound, double InitialPhase, unsigned Division, bool Precise);
			
			/// <summary>
            /// Calculates WaveForm of UVW in 1 sec.
            /// </summary>
            /// <param name="Control"></param>
            /// <param name="Sound"></param>
            /// <param name="InitialPhase"></param>
            /// <param name="Division"> Recommend : 120000 , Brief : 2000 </param>
            /// <param name="Precise"> True for more precise calculation when Freq < 1</param>
            /// <returns> WaveForm of UVW in 1 sec.</returns>
            QVector<WaveValues_UVW> GetUVWSec(VvvfValues& Control, YamlVvvfSoundData& Sound, double InitialPhase, unsigned Division, bool Precise);
            
            QVector<WaveValues_UVW> GetUVW(VvvfValues Control, YamlVvvfSoundData Sound, double InitialPhase, double InvDeltaT, unsigned Count);
		}
		
		namespace Fourier
		{
			constexpr double VoltageConvertFactor = 1.102657791;
			double GetFourier(const QVector<WaveValues_UVW>& UVW, unsigned N);
			
			double GetFourierFast(const QVector<WaveValues_UVW>& UVW, unsigned N);
			
			QVector<double> GetFourierCoefficients(const QVector<WaveValues_UVW>& UVW, unsigned N);
			
			/// <summary>
            /// Gets Fourier series coefficients
            /// </summary>
            /// <param name="Control">Make sure you put cloned data.</param>
            /// <param name="Sound"></param>
            /// <param name="Delta"></param>
            /// <param name="N"></param>
            /// <returns></returns>
            QVector<double> GetFourierCoefficients(VvvfValues& Control, YamlVvvfSoundData& Sound, int Delta, unsigned N);
            
            std::string GetDesmosFourierCoefficientsArray(QVector<double>& coefficients);
            
            /// <summary>
            /// Do clone about control!
            /// </summary>
            /// <param name="Sound"></param>
            /// <param name="Control"></param>
            /// <returns></returns>
            double GetVoltageRate(VvvfValues& Control, YamlVvvfSoundData& Sound, bool Precise, bool FixSign = true);
            double GetVoltageRate(ref QVector<WaveValues_UVW>, bool FixSign = true);
		}
	}
}