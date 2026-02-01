#include "BaseFrequency.hpp"
#include "Serialization.hpp"

// Standard Library
#include <sstream>

namespace VvvfSimulator::Data
{
	std::string BaseFrequency::Point::toString() const
	{
		std::ostringstream oss;
		oss << "Order: " << order 
		    << ", Rate: " << rate 
		    << ", Duration: " << duration 
		    << ", Brake: " << (brake ? "true" : "false")
		    << ", PowerOn: " << (powerOn ? "true" : "false");
		return oss.str();
	}

    BaseFrequency::BaseFrequency(const std::filesystem::path &path, RflCppFormats format)
    {
		auto result = load(path, format);
		if (result) {
			*this = std::move(result.value());
		} else {
			throw std::runtime_error("Failed to load BaseFrequency from file: " + result.error().message());
		}
    }

    std::string BaseFrequency::toString() const
    {
		std::ostringstream oss;
		oss << "BaseFrequency { Points: [";
		for (size_t i = 0; i < points.size(); ++i) {
			if (i > 0) oss << ", ";
			oss << "{" << points[i].toString() << "}";
		}
		oss << "] }";
		return oss.str();
	}

	double BaseFrequency::getEstimatedSteps(double sampleTime) const
	{
		// Compile and get total time from last point
		auto compiled = getCompiled();
		if (compiled.points.empty())
			return 0.0;
		
		double totalTime = compiled.points.back().endTime;
		return totalTime / sampleTime;
	}

	BaseFrequency::Compiled BaseFrequency::getCompiled() const
	{
		return Compiled(*this);
	}

	BaseFrequency::Compiled::Compiled(const BaseFrequency& source)
	{
		// Clone and sort points by order
		std::vector<BaseFrequency::Point> sortedPoints = source.points;
		std::sort(sortedPoints.begin(), sortedPoints.end(),
			[](const BaseFrequency::Point& a, const BaseFrequency::Point& b) {
				return a.order < b.order;
			});

		double currentTime = 0.0;
		double currentFrequency = 0.0;

		for (const auto& originalPoint : sortedPoints) {
			// Duration -1 means instant frequency change (no time passage)
			if (originalPoint.duration == -1) {
				currentFrequency = originalPoint.rate;
				continue;
			}

			double deltaTime = originalPoint.duration;
			double deltaFrequency = deltaTime * originalPoint.rate * (originalPoint.brake ? -1.0 : 1.0);

			// Skip zero-duration segments
			if (deltaTime == 0.0) continue;

			Point compiledPoint;
			compiledPoint.startTime = currentTime;
			compiledPoint.endTime = currentTime + deltaTime;
			compiledPoint.startFrequency = currentFrequency;
			compiledPoint.endFrequency = currentFrequency + deltaFrequency;
			compiledPoint.isPowerOn = originalPoint.powerOn;
			compiledPoint.isAccel = !originalPoint.brake;

			points.push_back(compiledPoint);

			currentTime += deltaTime;
			currentFrequency += deltaFrequency;
		}
	}

	// ===== Compiled Analysis Methods (ex-Analyze) =====

	int BaseFrequency::Compiled::getPointAtNum(double time) const
	{
		if (points.empty()) return -1;
		if (time < points.front().startTime || points.back().endTime < time) return -1;

		// Binary search for the point containing the given time
		int left = 0;
		int right = static_cast<int>(points.size()) - 1;

		while (left <= right) {
			int mid = left + (right - left) / 2;
			const Point& point = points[mid];

			if (point.startTime <= time && time < point.endTime) {
				return mid; // Found the point
			}

			if (point.startTime < time) {
				left = mid + 1;
			} else {
				right = mid - 1;
			}
		}

		return -1; // Out of range
	}

	std::optional<BaseFrequency::Compiled::Point> BaseFrequency::Compiled::getPointAtData(double time) const
	{
		int index = getPointAtNum(time);
		if (index < 0 || static_cast<size_t>(index) >= points.size())
			return std::nullopt;
		return points[index];
	}

	double BaseFrequency::Compiled::getFreqAt(double time, double initial) const
	{
		auto pointOpt = getPointAtData(time);
		if (!pointOpt)
			return initial;

		const Point& point = *pointOpt;

		// Linear interpolation between start and end frequency
		double frequencyChangeRate = (point.endFrequency - point.startFrequency) / 
		                              (point.endTime - point.startTime);
		double frequency = frequencyChangeRate * (time - point.startTime) + point.startFrequency;

		return frequency + initial;
	}

	// ===== Serialization Methods =====

	rfl::Result<rfl::Nothing> BaseFrequency::save(const std::filesystem::path& path, RflCppFormats format) const
	{
		return Serialization::save(*this, path, format);
	}

	rfl::Result<BaseFrequency> BaseFrequency::load(const std::filesystem::path& path, RflCppFormats format)
	{
		return Serialization::load<BaseFrequency>(path, format);
	}
}
