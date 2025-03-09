#include "Calculate.hpp"

// Standard Library
#include <algorithm>
#include <random>
// Internal
#include "InternalMath.hpp"

namespace NAMESPACE_VVVF::Calculate
{
	using namespace NAMESPACE_VVVF::InternalMath;
	
	double getBaseWaveform(const YamlPulseMode &mode, double x, double amplitude, double T, double initialPhase, bool* OK)
	{
		using BaseWaveType = YamlPulseMode::BaseWaveType;
		
		double baseValue;
		switch (mode.BaseWave)
		{
		case BaseWaveType::Sine:
			baseValue = InternalMath::Functions::sine(x);
			break;
		case BaseWaveType::Saw:
			baseValue = -(InternalMath::Functions::saw(x));
			break;
		case BaseWaveType::Square:
			baseValue = InternalMath::Functions::square(x);
			break;
		case BaseWaveType::ModifiedSineA:
			baseValue = InternalMath::Functions::modifiedSine(x, 1);
			break;
		case BaseWaveType::ModifiedSineB:
			baseValue = InternalMath::Functions::modifiedSine(x, 2);
			break;
		case BaseWaveType::ModifiedSawA:
			baseValue = InternalMath::Functions::modifiedSaw(x);
			break;
		default:
			if (OK != nullptr) *OK = false;
			return 0.0;
		}
		baseValue *= amplitude;

		for (const auto& harmonic : mode.PulseHarmonics)
		{
			double harmonic_x = harmonic.IsHarmonicProportional ?
				harmonic.Harmonic * (x + harmonic.InitialPhase) :
				m_2PI * harmonic.Harmonic * (T + initialPhase);
			double harmonicValue;
			switch (harmonic.Type)
			{
			case YamlPulseMode::PulseHarmonic::PulseHarmonicType::Sine:
				harmonicValue = InternalMath::Functions::sine(harmonic_x);
				break;
			case YamlPulseMode::PulseHarmonic::PulseHarmonicType::Saw:
				harmonicValue = -(InternalMath::Functions::saw(harmonic_x));
				break;
			case YamlPulseMode::PulseHarmonic::PulseHarmonicType::Square:
				harmonicValue = InternalMath::Functions::square(harmonic_x);
				break;
			default:
				if (OK != nullptr) *OK = false;	
				return 0.0;
			}
			baseValue += harmonicValue * harmonic.Amplitude * (harmonic.IsAmplitudeProportional ? amplitude : 1);
		}

		baseValue = std::clamp(baseValue, -1.0, 1.0);
		
		if (OK != nullptr) *OK = true;
		return baseValue;
	}
	double discreteTimeLine(double x, int level, DiscreteTimeConfiguration::DiscreteTimeMode mode, bool* OK)
	{
		const double seed = std::fmod(x, m_2PI) * level * m_1_2PI;
		double time;
		switch (mode)
		{
		case DiscreteTimeConfiguration::DiscreteTimeMode::Left:
			time = std::ceil(seed);
			break;
		case DiscreteTimeConfiguration::DiscreteTimeMode::Middle:
			time = std::round(seed);
			break;
		case DiscreteTimeConfiguration::DiscreteTimeMode::Right:
			time = std::floor(seed);
			break;
		default:
			if (OK != nullptr) *OK = false;	
			return 0.0;
		}
		if (OK != nullptr) *OK = true;	
		return time * m_2PI / level;
	}
	int_fast8_t getHOP(double x, double amplitude, int_fast8_t carrier, int_fast8_t width)
	{
		int_fast16_t totalSteps = carrier * 2;
		const double fixed_x = std::fmod(x, m_PI) * totalSteps * m_1_PI,
		             sawValue = -InternalMath::Functions::saw(carrier * x);
		double modulated;
		     if (fixed_x > totalSteps - 1) modulated = -1.0;
		else if (fixed_x > totalSteps / 2 + width) modulated = 1.0;
		else if (fixed_x > totalSteps / 2 - width) modulated = 2.0 * amplitude - 1.0;
		else if (fixed_x > 1) modulated = 1.0;
		else    modulated = -1;
		     if (std::fmod(x, m_2PI) > m_PI) modulated = -modulated;

		return modulateSignal(modulated, sawValue);
	}
	double getAmplitude(const AmplitudeParameter &param, double current)
	{
		double amplitude = 0.0;

		if (param.EndAmplitude == param.StartAmplitude) amplitude = param.StartAmplitude;
		else switch (param.Mode)
		{
		case AmplitudeParameter::AmplitudeMode::Linear:
			if (!param.DisableRangeLimit) current = std::clamp(current, param.StartFrequency, param.EndFrequency);
			amplitude = (param.EndAmplitude - param.StartAmplitude) / (param.EndFrequency - param.StartFrequency) * (current - param.StartFrequency) + param.StartAmplitude;
			break;
		case AmplitudeParameter::AmplitudeMode::InverseProportional:
			if (!param.DisableRangeLimit) current = std::clamp(current, param.StartFrequency, param.EndFrequency);

			const double  x = (1.0 / param.EndAmplitude - 1.0 / param.StartAmplitude) / (param.EndFrequency - param.StartFrequency) * (current - param.StartFrequency) + 1.0 / param.StartAmplitude;

			const double  c = -param.CurveChangeRate;
			const double& k = param.EndAmplitude;
			const double& l = param.StartAmplitude;
			const double  a = 1.0 / (1.0 / l - 1.0 / k) * (1.0 / (l - c) - 1.0 / (k - c));
			const double  b = 1.0 / (1.0 - 1.0 / l * k) * (1.0 / (l - c) - 1.0 / l * k / (k - c));

			amplitude = 1 / (a * x + b) + c;
			break;
		case AmplitudeParameter::AmplitudeMode::Exponential:
			if (!param.DisableRangeLimit && current > param.EndFrequency) current = param.EndFrequency;
			const double t = 1.0 / param.EndFrequency * std::log(param.EndAmplitude + 1.0);
			amplitude = std::pow(m_E, t * current) - 1.0;
			break;
		case AmplitudeParameter::AmplitudeMode::LinearPolynomial:
			if (!param.DisableRangeLimit && current > param.EndFrequency) current = param.EndFrequency;
			amplitude = std::pow(current, param.Polynomial) / std::pow(param.EndFrequency, param.Polynomial) * param.EndAmplitude;
			break;
		case AmplitudeParameter::AmplitudeMode::Sine:
			if (!param.DisableRangeLimit && current > param.EndFrequency) current = param.EndFrequency;
			const double x = m_PI * current / (2.0 * param.EndFrequency);
			amplitude = InternalMath::Functions::sine(x) * param.EndAmplitude;
			break;
		case AmplitudeParameter::AmplitudeMode::Table:
			auto targetIt = param.AmplitudeTable.cbegin();
			for (auto it = param.AmplitudeTable.cbegin(); it != param.AmplitudeTable.cend(); ++it)
			{
				if (it->Frequency > current) break;
				targetIt = it;
			}

			if (param.AmplitudeTableInterpolation && (targetIt + 1) != param.AmplitudeTable.cend())
			{
				const double &frequencyStart = targetIt->Frequency,
				             &frequencyEnd = (targetIt + 1)->Frequency,
				             &amplitudeStart = targetIt->Amplitude,
				             &amplitudeEnd = (targetIt + 1)->Amplitude;
				amplitude = (amplitudeEnd - amplitudeStart) / (frequencyEnd - frequencyStart) * (current - frequencyStart) + amplitudeStart;
			}
			else amplitude = targetIt->Amplitude;
		}

		if (param.CutOffAmplitude > amplitude) amplitude = 0.0;
		if (param.MaxAmplitude != -1.0 && param.MaxAmplitude < amplitude) amplitude = param.MaxAmplitude;

		return amplitude;
	}
	double getRandomFrequency(const CarrierFreq &data, VvvfValues &control)
	{
		if (data.range == 0.0 || !control.allowRandomFreqMove) return data.baseFrequency;

		double randomFreq;
		if (control.randomFreqPreviousTime == 0.0)
		{
			const double halfRange = data.range * 0.5;
			const double diffFreq = std::uniform_real_distribution<double>(-halfRange, halfRange)(control.rnd);
			const double silentRandomFreq = data.baseFrequency + diffFreq;
			randomFreq = silentRandomFreq;
			control.previousSawRandomFreq = silentRandomFreq;
			control.randomFreqPreviousTime = control.generationCurrentTime;
		}
		else randomFreq = control.previousSawRandomFreq;

		if (control.randomFreqPreviousTime + data.interval < control.generationCurrentTime)
			control.randomFreqPreviousTime = 0.0;

		return randomFreq;
	}
	double getPeriodicFrequency(double lowest, double highest, double intervalTime, bool continuous, VvvfValues &control)
	{
		if (!control.allowRandomFreqMove) return (highest + lowest) * 0.5;

		double periodicFreq;
		const double& current_t = control.generationCurrentTime, solve_t = control.periodicFreqPreviousTime;

		if (continuous)
		{
			if (intervalTime * 0.5 > current_t - solve_t)
				periodicFreq = lowest + (highest - lowest) / (intervalTime * 0.5) * (current_t - solve_t);
			else
				periodicFreq = highest + (lowest - highest) / (intervalTime * 0.5) * (current_t - solve_t - intervalTime * 0.5);
		}
		else periodicFreq = (intervalTime * 0.5 > current_t - solve_t) ? highest : lowest;

		if (current_t - solve_t > intervalTime) control.periodicFreqPreviousTime = current_t;
		return periodicFreq;
	}
	WaveValues calculatePhases(VvvfValues &control, const PwmCalculateValues &value, double addInitial, std::array<std::exception_ptr, 3> *ePtrs)
	{
		if (control.getSinFreq() < value.minimumFrequency && control.controlFrequency > 0.0)
			control.videoSineFrequency = value.minimumFrequency;
		else control.videoSineFrequency = control.getSinFreq();

		if (value.none) return WaveValues(0.0, 0.0, 0.0);

		control.videoPulseMode = value.pulseMode;
		control.videoSineAmplitude = value.amplitude;
		//if (!value.carrier)
			control.videoCarrierFrequency = value.carrier;
		control.videoPulseData = value.pulseData;

		int_fast8_t U = 0, V = 0, W = 0;

		for (int i = 0; i < 3; i++)
		{
			int_fast8_t val;
			double initial = m_2PI / 3.0 * i + addInitial;
			try
			{
				val = value.level == 2 ? calculate2Level(control, value, initial) : calculate3Level(control, value, initial);
			}
			catch(...)
			{
				if (ePtrs != nullptr) (*ePtrs)[i] = std::current_exception();
				val = 0;
			}
			switch (i)
			{
			case 0: U = val; break;
			case 1: V = val; break;
			default: W = val; break; // case 2
			}
		}

		return WaveValues(U, V, W);
	}
}