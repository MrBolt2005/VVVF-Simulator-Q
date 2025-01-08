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
#include <expected>
#include <string>
#include <type_traits>
#include <typeinfo>

// If compiling on GCC or Clang:
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#endif // __has_include(<cxxabi.h>)

// If compiling on MSVC:
#if __has_include(<dbghelp.h>)
#include <cstring>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#endif // __has_include(<dbghelp.h>)

#if __has_included(<errhandlingapi.h>)
#include <errhandlingapi.h>
#endif // __has_include(<errhandlingapi.h>)

#if __has_include(<windows.h>)
#include <windows.h>
#endif // __has_include(<windows.h>)

namespace VvvfSimulator::Logging
{
	template <typename Callable, typename = std::enable_if_t<std::is_invocable_v<Callable>>>
	std::expected<std::string, int> getDemangledCallableSignature(Callable callable)
	{
		#if __has_include(<windows.h>) && __has_include(<dbghelp.h>)
		// Code for the MSVC ABI
		std::array<char, 256> demangledName_arr;
		const auto mangledName = typeid(callable).name();
		int result = UnDecorateSymbolName(mangledName, demangledName_arr, demangledName_arr.size(), UNDNAME_COMPLETE);
		if (result != 0)
			return std::string(demangledName_arr);
		else return std::unexpected(GetLastError());
		#elif __has_include(<cxxabi.h>)
		// Code for the libstdc++ ABI
		int status;
		auto demangledName_c_str = abi::__cxa_demangle(typeid(callable).name(), NULL, NULL, &status);
		if (status == 0)
		{
			std::string demangledName(demangledName_c_str);
			std::free(demangledName_c_str);
			// Definitely **not** needed, but maybe we could play it safe anyways and it still has "negligible" runtime costs, whatsoever:
			//demangledName_c_str = nullptr;
			return demangledName;
		}
		else
		{
			std::free(demangledName_c_str);
			//demangledName_c_str = nullptr;
			return std::unexpected(status);
		}
		#else
		// This should not happen(!)
		return typeid(callable).name();
        #endif
	}
}
