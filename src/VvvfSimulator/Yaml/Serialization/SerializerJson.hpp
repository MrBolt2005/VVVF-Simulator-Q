#ifndef SERIALIZERJSON_HPP
#define SERIALIZERJSON_HPP

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

// SerializerJson.hpp
// Version 1.9.1.1

// Internal Includes
#include "Serializer.hpp"
// Package Includes
#include <rfl/json.hpp>

namespace VvvfSimulator::Yaml::Serialization
{
	template <>
	struct Serializer<yyjson_mut_doc*>
	{
		template <typename T>
		static yyjson_mut_doc* serialize(const T& object)
        {
            yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
            yyjson_mut_val* root = yyjson_mut_obj(doc);
            yyjson_mut_doc_set_root(doc, root);

            rfl::for_each(object, [&root, &doc](auto& field)
            {
                yyjson_mut_obj_add_str(doc, root, field.name, field.get().c_str());
            });

            return doc;
        }

		template <typename T>
		static T deserialize(yyjson_doc* doc)
        {
            T object;
            yyjson_val* root = yyjson_doc_get_root(doc);

            rfl::for_each(object, [&root](auto& field)
            {
                const char* value = yyjson_obj_get_str(root, field.name);
                field.set(std::string(value));
            })
            
            return object;
        }
	};
}

#endif // SERIALIZERJSON_HPP
