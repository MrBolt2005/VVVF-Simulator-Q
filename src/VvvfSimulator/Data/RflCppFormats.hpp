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

// RflCppFormats.hpp
// Version 1.9.1.1

namespace VvvfSimulator::Data
{
	// reflect-cpp supported formats gotten from https://rfl.getml.com/
	enum class RflCppFormats:char
	{
		Avro = 'A',
		BSON = 'B',
		CBOR = 'C',
		FlexBuffers = 'F',
		JSON = 'J',
		JSON_Schema = 'j',
		MessagePack = 'M',
		TOML = 'T',
		UBJSON = 'U',
		XML = 'X',
		YAML = 'Y'
	};

	constexpr auto toString(RflCppFormats format) noexcept
	{
		switch (format)
		{
		case RflCppFormats::Avro:
			return "Avro";
		case RflCppFormats::BSON:
			return "BSON";
		case RflCppFormats::CBOR:
			return "CBOR";
		case RflCppFormats::FlexBuffers:
			return "FlexBuffers";
		case RflCppFormats::JSON:
			return "JSON";
		case RflCppFormats::JSON_Schema:
			return "JSON Schema";
		case RflCppFormats::MessagePack:
			return "MessagePack";
		case RflCppFormats::TOML:
			return "TOML";
		case RflCppFormats::UBJSON:
			return "UBJSON";
		case RflCppFormats::XML:
			return "XML";
		case RflCppFormats::YAML:
			return "YAML";
		default:
			return "Unknown";
		}
	}
}
