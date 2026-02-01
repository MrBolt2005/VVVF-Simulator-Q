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

// Data/Util.hpp
// Version 1.10.0.0

// Standard Library Includes
#include <concepts>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace VvvfSimulator::Data
{
	namespace Util
	{
		// Helper to get property values as string (for debugging/logging)
		template<typename T>
		std::string GetPropertyValue(const T& value)
		{
			std::ostringstream oss;
			
			if constexpr (std::is_same_v<T, std::string>) {
				oss << value;
			} else if constexpr (std::is_arithmetic_v<T>) {
				oss << value;
			} else if constexpr (requires { value.begin(); value.end(); }) {
				// Container-like
				oss << "[";
				bool first = true;
				for (const auto& item : value) {
					if (!first) oss << ", ";
					oss << GetPropertyValue(item);
					first = false;
				}
				oss << "]";
			} else {
				oss << "object";
			}
			
			return oss.str();
		}
	}
}
