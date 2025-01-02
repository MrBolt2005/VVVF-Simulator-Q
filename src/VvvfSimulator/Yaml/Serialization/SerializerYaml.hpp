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
#include "Serializer.hpp"
// Package Includes
#include <rfl/yaml.hpp>

namespace VvvfSimulator::Yaml::Serialization
{
	template <>
	struct Serializer<YAML::Node>
	{
		template <typename T>
		static YAML::Node serialize(const T& object)
        {
            YAML::Node node;
            rfl::for_each(object, [&node](auto& field)
            {
                node[field.name] = field.get();
            });
            return node;
        }

		template <typename T>
		static T deserialize(const YAML::Node& node)
        {
            T object;
            rfl::for_each(object, [&node](auto& field)
            {
                field.set(node[field.name].as<std::decay_t<decltype(field.get())>>());
            });
            return object;
        }
	};
}

#endif // SERIALIZATION_HPP
