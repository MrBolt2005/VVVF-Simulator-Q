#include "Struct.hpp"

// Standard Library
#include <algorithm>
#include <sstream>

namespace VvvfSimulator::Data::Vvvf
{
	void Struct::sortAcceleratePattern(bool inverse)
	{
		std::sort(acceleratePattern.begin(), acceleratePattern.end(),
			[inverse](const PulseControl& a, const PulseControl& b) {
				return inverse ? 
					(a.controlFrequencyFrom < b.controlFrequencyFrom) :
					(a.controlFrequencyFrom > b.controlFrequencyFrom);
			});
	}

	void Struct::sortBrakingPattern(bool inverse)
	{
		std::sort(brakingPattern.begin(), brakingPattern.end(),
			[inverse](const PulseControl& a, const PulseControl& b) {
				return inverse ? 
					(a.controlFrequencyFrom < b.controlFrequencyFrom) :
					(a.controlFrequencyFrom > b.controlFrequencyFrom);
			});
	}

	bool Struct::hasCustomPwm() const
	{
		auto hasCustom = [](const PulseControl& pc) {
			using PT = PulseControl::Pulse::PulseTypeName;
			return pc.pulseMode.pulseType == PT::CHM || 
			       pc.pulseMode.pulseType == PT::SHE;
		};

		return std::any_of(acceleratePattern.begin(), acceleratePattern.end(), hasCustom) ||
		       std::any_of(brakingPattern.begin(), brakingPattern.end(), hasCustom);
	}

	std::string Struct::toString() const
	{
		std::ostringstream oss;
		oss << "Vvvf::Struct { "
		    << "Level: " << level
		    << ", AccelPatterns: " << acceleratePattern.size()
		    << ", BrakePatterns: " << brakingPattern.size()
		    << ", HasCustomPwm: " << (hasCustomPwm() ? "true" : "false")
		    << " }";
		return oss.str();
	}
}

// ===== Analysis Methods (ex-Analyze) =====

template<typename TPattern>
std::optional<TPattern> Struct::findCurrentPattern(
const std::vector<TPattern>& patterns,
double controlFrequency
) const {
if (patterns.empty()) return std::nullopt;

for (auto it = patterns.rbegin(); it != patterns.rend(); ++it) {
if (controlFrequency >= it->controlFrequencyFrom) return *it;
}
return patterns.front();
}

ControlFrequencyData Struct::getControlFrequencyData(double controlFrequency) const
{
ControlFrequencyData data;
auto accelPattern = getCurrentAcceleratePattern(controlFrequency);
auto brakePattern = getCurrentBrakingPattern(controlFrequency);

if (accelPattern) {
data.waveForm = accelPattern->control.waveForm;
data.amplitude = accelPattern->control.amplitude;
data.carrierFrequency = accelPattern->control.carrierFrequency;
data.dipolar = accelPattern->control.dipolar;
data.pulseMode = accelPattern->control.pulseMode;
} else if (brakePattern) {
data.waveForm = brakePattern->control.waveForm;
data.amplitude = brakePattern->control.amplitude;
data.carrierFrequency = brakePattern->control.carrierFrequency;
data.dipolar = brakePattern->control.dipolar;
data.pulseMode = brakePattern->control.pulseMode;
} else {
data.waveForm = WaveForm::Sine;
data.amplitude = 0.0;
data.carrierFrequency = 0.0;
data.dipolar = 3;
data.pulseMode = PulseMode::PWM;
}
return data;
}

std::optional<AcceleratePattern> Struct::getCurrentAcceleratePattern(double controlFrequency) const
{ return findCurrentPattern(acceleratePattern, controlFrequency); }

std::optional<BrakingPattern> Struct::getCurrentBrakingPattern(double controlFrequency) const
{ return findCurrentPattern(brakingPattern, controlFrequency); }

double Struct::getFreeRunFrequency(double controlFrequency) const
{
auto accelPattern = getCurrentAcceleratePattern(controlFrequency);
if (accelPattern && accelPattern->control.freeRunOption.enable)
return controlFrequency * accelPattern->control.freeRunOption.sineFrequencyMultiplier;

auto brakePattern = getCurrentBrakingPattern(controlFrequency);
if (brakePattern && brakePattern->control.freeRunOption.enable)
return controlFrequency * brakePattern->control.freeRunOption.sineFrequencyMultiplier;

return controlFrequency;
}

double Struct::getMinimumFrequency() const
{
double minFreq = 0.0;
if (!acceleratePattern.empty()) minFreq = acceleratePattern.front().controlFrequencyFrom;
if (!brakingPattern.empty()) {
double brakeMin = brakingPattern.front().controlFrequencyFrom;
if (minFreq == 0.0 || brakeMin < minFreq) minFreq = brakeMin;
}
return minFreq;
}

double Struct::getMaximumFrequency() const
{
double maxFreq = 0.0;
for (const auto& pattern : acceleratePattern) 
if (pattern.controlFrequencyFrom > maxFreq) maxFreq = pattern.controlFrequencyFrom;
for (const auto& pattern : brakingPattern)
if (pattern.controlFrequencyFrom > maxFreq) maxFreq = pattern.controlFrequencyFrom;
return maxFreq;
}

bool Struct::requiresCustomPwm(double controlFrequency) const
{
auto data = getControlFrequencyData(controlFrequency);
return data.pulseMode == PulseMode::CHM || data.pulseMode == PulseMode::SHE;
}

double Struct::getAmplitude(double controlFrequency) const
{ return getControlFrequencyData(controlFrequency).amplitude; }

PulseMode Struct::getPulseMode(double controlFrequency) const
{ return getControlFrequencyData(controlFrequency).pulseMode; }

double Struct::getCarrierFrequency(double controlFrequency) const
{ return getControlFrequencyData(controlFrequency).carrierFrequency; }

bool Struct::isAsyncMode(double controlFrequency) const
{
auto accelPattern = getCurrentAcceleratePattern(controlFrequency);
if (accelPattern && accelPattern->control.asyncControl.enable) return true;
auto brakePattern = getCurrentBrakingPattern(controlFrequency);
return brakePattern && brakePattern->control.asyncControl.enable;
}

int Struct::getDipolar(double controlFrequency) const
{ return getControlFrequencyData(controlFrequency).dipolar; }
}
