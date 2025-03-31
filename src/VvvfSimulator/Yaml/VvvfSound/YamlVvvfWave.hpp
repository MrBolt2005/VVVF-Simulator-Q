#pragma one

// Standard Library
#include <cmath>
// Internal
#include "../../Vvvf/InternalMath.hpp"
#include "../../Vvvf/Struct.hpp"
#include "Namespace_YamlVvvfSound.h"
#include "YamlVvvfAnalyze.hpp"

namespace NAMESPACE_YAMLVVVFSOUND::YamlVvvfWave
{
	using NAMESPACE_VVVF::Struct::VvvfValues;
	
	inline constexpr auto getChangingValue(auto x1, auto y1, auto x2, auto y2, auto x) noexcept
	{
		return y1 + (y2 - y1) / (x2 - x1) * (x - x1);
	}

	inline double getMovingValue(const YamlVvvfSoundData::YamlControlData::YamlMovingValue &data, double current) noexcept
	{
		switch (data.Type)
		{
		case YamlVvvfSoundData::YamlControlData::YamlMovingValue::MovingValueType::Proportional:
			return getChangingValue(
				data.Start,
				data.StartValue,
				data.End,
				data.EndValue,
				current
			);
			break;
		case YamlVvvfSoundData::YamlControlData::YamlMovingValue::MovingValueType::Pow2_Exponential:
			return std::pow(
				2.0,
				std::pow(
					(current - data.Start) / (data.End - data.Start), data.Degree
				) - 1.0
			) * (data.EndValue - data.StartValue) + data.StartValue;
			break;
		case YamlVvvfSoundData::YamlControlData::YamlMovingValue::MovingValueType::Inv_Proportional:
			const double x = getChangingValue(
				data.Start,
				1.0 / data.StartValue,
				data.End,
				1.0 / data.EndValue,
				current
			);

			const double c = -data.CurveRate;
			const double &k = data.EndValue;
			const double &l = data.StartValue;
			const double a = 1 / ((1 / l) - (1 / k)) * (1 / (l - c) - 1 / (k - c));
			const double b = 1 / (1 - (1 / l) * k) * (1 / (l - c) - (1 / l) * k / (k - c));

			return 1 / (a * x + b) + c;
			break;
		case YamlVvvfSoundData::YamlControlData::YamlMovingValue::MovingValueType::Sine:
			const double x = (NAMESPACE_VVVF::InternalMath::m_PI_2 - std::asin(data.StartValue / data.EndValue)) / (data.End - data.Start) * (current - data.Start) + std::asin(data.StartValue / data.EndValue);
			return std::sin(x) * data.EndValue;
			break;
		default:
			return 1000.0;
		}
	}

	bool isMatching (const VvvfValues &control, const YamlVvvfSoundData::YamlControlData &ysd);

	NAMESPACE_VVVF::Struct::PwmCalculateValues calculateYaml(const VvvfValues &control, const YamlVvvfSoundData &yvs);
}