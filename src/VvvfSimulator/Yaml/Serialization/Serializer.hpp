#ifndef SERIALIZATION_HPP
#define SERIALIZATION_HPP

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

// Serialization.hpp
// Version 1.9.1.1

// Internal Includes

// Package Includes
#include <rfl.hpp>

namespace VvvfSimulator::Yaml::Serialization
{
	// Yeah, despite this namespace being called YAML, we're actually supporting other formats here now.
	// reflect-cpp for the win! ^.^

	// Generic Interface
	template <typename Format>
	struct Serializer
	{
		template <typename T>
		static auto serialize(const T& object);

		template <typename T>
		static T deserialize(const auto& container);
	};
}

#endif // SERIALIZATION_HPP
