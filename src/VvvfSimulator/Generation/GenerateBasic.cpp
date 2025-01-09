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
			QVector<WaveValues_UVW> getUVWCycle(VvvfValues& control, YamlVvvfSoundData& sound, double initialPhase, unsigned division, bool precise)
			{
				PwmCalculateValues preCalculate = YamlVvvfWave::calculateYaml(control, sound);
				double _F = control.sinAngleFreq;
				if (_F < preCalculate.minimumFrequency && control.controlFrequency > 0) _F = preCalculate.minimumFrequency;
				double _K = (_F > 0.01 && _F < 1) ? 1 / _F : 1;

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
				
				return getUVW(control, sound, initialPhase, invDeltaT, count);
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
			QVector<WaveValues> getUVWSec(VvvfValues& control, YamlVvvfSoundData& sound, double initialPhase, unsigned division, bool precise)
			{
				PwmCalculateValues preCalculate = YamlVvvfWave::calculateYaml(control, sound);
				double _F = control.sinAngleFreq;
				if (_F < preCalculate.minimumFrequency && control.controlFrequency > 0) _F = preCalculate.minimumFrequency;
				double _K = (_F > 0.01 && _F < 1) ? 1 / _F : 1;

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
				
				return getUVW(control, sound, initialPhase, count, count);
			}
		}
	}
}
