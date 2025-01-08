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

// Logging.hpp
// Version 1.9.1.1

// Standard Library Includes
#include <array>
#include <cstring>
#include <typeinfo>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#endif // __has_include(<cxxabi.h>)

#if __has_include(<dbghelp.h>)
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif // __has_include(<dbghelp.h>)

#if __has_include(<windows.h>)
#include <windows.h>
#endif // __has_include(<windows.h>)

namespace VvvfSimulator::Logging
{
	template <typename Callable>
	std::string getDemangledCallableSignature(Callable callable, int *status = nullptr)
	{
		#if __has_include(<windows.h>) && __has_include(<dbghelp.h>)
		// Code for the MSVC ABI
		std::array<char, 256> demangledName_arr;
		UnDecorateSymbolName(typeid(callable).name(), demangledName_arr, demangledName_arr.size(), UNDNAME_COMPLETE);
		return std::string(demangledName_arr);
		#elif __has_include(<cxxabi.h>)
		// Code for the libstdc++ ABI
		if (!status)
		{
			int _status;
			status = &_status;
		}
		return abi::__cxa_demangle(typeid(callable).name(), NULL, NULL, status);
		#else
		// This should not happen(!)
		return typeid(callable).name();
	}
}
