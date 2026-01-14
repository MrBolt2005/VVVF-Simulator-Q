#include "Struct.hpp"

// Standard Library
#include <sstream>

namespace VvvfSimulator::Data::BaseFrequency
{
	std::string Point::toString() const
	{
		std::ostringstream oss;
		oss << "Order: " << order 
		    << ", Rate: " << rate 
		    << ", Duration: " << duration 
		    << ", Brake: " << (brake ? "true" : "false")
		    << ", PowerOn: " << (powerOn ? "true" : "false");
		return oss.str();
	}

	std::string Struct::toString() const
	{
		std::ostringstream oss;
		oss << "BaseFrequency::Struct { Points: [";
		for (size_t i = 0; i < points.size(); ++i) {
			if (i > 0) oss << ", ";
			oss << "{" << points[i].toString() << "}";
		}
		oss << "] }";
		return oss.str();
	}

	double Struct::getEstimatedSteps(double sampleTime) const
	{
		double totalDuration = 0.0;
		for (const auto& point : points) {
			if (point.duration > 0.0)
				totalDuration += point.duration;
		}
		return totalDuration / sampleTime;
	}

	Struct::StructCompiled Struct::getCompiled() const
	{
		return StructCompiled(*this);
	}

	Struct::StructCompiled::StructCompiled(const Struct& source)
		: compiledPoints(source.points)
	{
		// Future optimization: pre-compute interpolation coefficients, etc.
	}

	// ===== StructCompiled Analysis Methods (ex-Analyze) =====

	int Struct::StructCompiled::getPointAtNum(double time) const
	{
		// Calculate cumulative times to find the point
		double cumulativeTime = 0.0;
		for (size_t i = 0; i < compiledPoints.size(); ++i) {
			double pointEndTime = cumulativeTime + compiledPoints[i].duration;
			if (time >= cumulativeTime && time < pointEndTime)
				return static_cast<int>(i);
			cumulativeTime = pointEndTime;
		}
		
		return -1; // Out of range
	}

	std::optional<Point> Struct::StructCompiled::getPointAtData(double time) const
	{
		int index = getPointAtNum(time);
		if (index < 0 || static_cast<size_t>(index) >= compiledPoints.size())
			return std::nullopt;
		return compiledPoints[index];
	}

	double Struct::StructCompiled::getFreqAt(double time, double initial) const
	{
		auto pointOpt = getPointAtData(time);
		if (!pointOpt)
			return initial;
			
		const Point& point = *pointOpt;
		
		// Calculate cumulative start time for this point
		double startTime = 0.0;
		for (size_t i = 0; i < compiledPoints.size(); ++i) {
			if (compiledPoints[i].order == point.order)
				break;
			startTime += compiledPoints[i].duration;
		}
		
		double elapsedInPoint = time - startTime;
		double frequency = point.rate * elapsedInPoint;
		
		return frequency + initial;
	}
}
