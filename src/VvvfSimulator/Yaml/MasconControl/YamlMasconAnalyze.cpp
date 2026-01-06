// Internal - Need to include it first because it includes the following file,
// otherwise it will cause a circular dependency and error out during namespace resolution:
#include "YamlMasconMidi.hpp"
#include "YamlMasconAnalyze.hpp"

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

// YamlMasconAnalyze.cpp
// Version 1.9.1.1

// Standard Library

// Packages
#include <libremidi.hpp>
#include <libremidi/reader.hpp>
// Internal