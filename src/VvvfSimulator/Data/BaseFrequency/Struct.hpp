#pragma once

/*
   Copyright © 2025 VvvfGeeks, VVVF Systems
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// Data/BaseFrequency/Struct.hpp
// Version 1.10.0.0

// Standard Library
#include <string>
#include <vector>
// Internal
#include "../Util.hpp"

namespace VvvfSimulator::Data::BaseFrequency
{
	/// Point in base frequency control pattern
	struct Point
	{
		int order = 0;
		double rate = 0.0;      // Hz/s - frequency change rate
		double duration = 0.0;  // seconds
		bool brake = false;      // true if braking pattern
		bool powerOn = true;     // true if power is on (renamed from masconOn)

		constexpr Point() = default;
		constexpr Point(int Order) : order(Order) {}
		constexpr Point(int Order, double Rate, double Duration, bool Brake, bool PowerOn)
			: order(Order), rate(Rate), duration(Duration), brake(Brake), powerOn(PowerOn) {}

		std::string toString() const;
	};

	/// Base frequency control data structure
	struct Struct
	{
		std::vector<Point> points;

		Struct() = default;
		Struct(const Struct& other) = default;
		Struct(Struct&& other) = default;
		Struct& operator=(const Struct& other) = default;
		Struct& operator=(Struct&& other) = default;

		std::string toString() const;
		
		/// Get estimated number of simulation steps for given sample time
		double getEstimatedSteps(double sampleTime) const;
		
		// Forward declaration for compiled structure
		struct StructCompiled;
		StructCompiled getCompiled() const;
	};

	/// Compiled/optimized version of base frequency data
	struct Struct::StructCompiled
	{
		// TODO: Add compiled fields as needed
		std::vector<Point> compiledPoints;

		explicit StructCompiled(const Struct& source);

		// ===== Analysis Methods (ex-Analyze) =====
		
		/// Get point index at given time using binary search
		int getPointAtNum(double time) const;
		
		/// Get point data at given time
		std::optional<Point> getPointAtData(double time) const;
		
		/// Get frequency at given time with initial offset
		double getFreqAt(double time, double initial) const;
		
		// TODO: Add checkForFreqChange when Domain is available
	};

	// Backward compatibility aliases
	using YamlMasconDataPoint = Point;
	using YamlMasconData = Struct;
}
