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

// Data/Serialization.hpp
// Version 1.10.0.0 - Generic serialization helpers

#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include "RflCppFormats.hpp"
#include <rfl.hpp>
#include <rfl/avro.hpp>
#include <rfl/bson.hpp>
#include <rfl/cbor.hpp>
#include <rfl/flexbuf.hpp>
#include <rfl/json.hpp>
#include <rfl/msgpack.hpp>
#include <rfl/toml.hpp>
#include <rfl/ubjson.hpp>
#include <rfl/xml.hpp>
#include <rfl/yaml.hpp>

namespace VvvfSimulator::Data::Serialization {

/// Generic save function using reflect-cpp
template<typename T>
rfl::Result<rfl::Nothing> save(const T& data, const std::filesystem::path& path, RflCppFormats format) {
    try {
        std::ofstream file(path, std::ios::binary);
        if (!file) {
            return rfl::Error("Failed to open file for writing: " + path.string());
        }

        std::string serialized;
        
        switch (format) {
            case RflCppFormats::YAML:
                serialized = rfl::yaml::write(data);
                break;
            case RflCppFormats::JSON:
                serialized = rfl::json::write(data);
                break;
            case RflCppFormats::TOML:
                serialized = rfl::toml::write(data);
                break;
            case RflCppFormats::XML:
                serialized = rfl::xml::write(data);
                break;
            case RflCppFormats::BSON:
                serialized = rfl::bson::write(data);
                break;
            case RflCppFormats::CBOR:
                serialized = rfl::cbor::write(data);
                break;
            case RflCppFormats::MessagePack:
                serialized = rfl::msgpack::write(data);
                break;
            case RflCppFormats::UBJSON:
                serialized = rfl::ubjson::write(data);
                break;
            case RflCppFormats::FlexBuffers:
                serialized = rfl::flexbuf::write(data);
                break;
            case RflCppFormats::Avro:
                serialized = rfl::avro::write(data);
                break;
            default:
                return rfl::Error("Unsupported format: " + std::string(toString(format)));
        }

        file.write(serialized.data(), serialized.size());
        return rfl::Nothing{};
    } catch (const std::exception& e) {
        return rfl::Error(std::string("Serialization error: ") + e.what());
    }
}

/// Generic load function using reflect-cpp
template<typename T>
rfl::Result<T> load(const std::filesystem::path& path, RflCppFormats format) {
    try {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return rfl::Error("Failed to open file for reading: " + path.string());
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        switch (format) {
            case RflCppFormats::YAML:
                return rfl::yaml::read<T>(content);
            case RflCppFormats::JSON:
                return rfl::json::read<T>(content);
            case RflCppFormats::TOML:
                return rfl::toml::read<T>(content);
            case RflCppFormats::XML:
                return rfl::xml::read<T>(content);
            case RflCppFormats::BSON:
                return rfl::bson::read<T>(content);
            case RflCppFormats::CBOR:
                return rfl::cbor::read<T>(content);
            case RflCppFormats::MessagePack:
                return rfl::msgpack::read<T>(content);
            case RflCppFormats::UBJSON:
                return rfl::ubjson::read<T>(content);
            case RflCppFormats::FlexBuffers:
                return rfl::flexbuf::read<T>(content);
            case RflCppFormats::Avro:
                return rfl::avro::read<T>(content);
            default:
                return rfl::Error("Unsupported format: " + std::string(toString(format)));
        }
    } catch (const std::exception& e) {
        return rfl::Error(std::string("Deserialization error: ") + e.what());
    }
}

} // namespace VvvfSimulator::Data::Serialization
