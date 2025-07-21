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

// Standard Library
#include <array>
#include <cstdlib>
#include <memory>
#include <string>
#include <type_traits>
#include <typeinfo>

// Packages
#include <QDebug>
#include <QMessageBox>
#include <QtGlobal>
#include <QWidget>

// Internal
#include "Outcome.hpp"

// If compiling on MSVC:
#if defined(_MSC_VER)

//#define DEMANGLED_CURRENT_FUNCTION_SIGNATURE __FUNCSIG__

#include <cstring>

#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#include <errhandlingapi.h>
#include <windows.h>

// If compiling on GCC or Clang:
#elif __has_include(<cxxabi.h>) // defined(__GNUC__) || defined(__clang__)

//#define DEMANGLED_CURRENT_FUNCTION_SIGNATURE __PRETTY_FUNCTION__

#include <cxxabi.h>

#endif

namespace VvvfSimulator::Logging
{
	template <typename Callable, typename = std::enable_if_t<std::is_invocable_v<Callable>>>
	inline Outcome::Result<std::string, int> getDemangledCallableSignature(Callable callable)
	{
		#if defined(_MSC_VER)

		// Code for the MSVC ABI
		std::array<char, 512> demangledName_arr;
		const auto mangledName = typeid(callable).name();
		int result = UnDecorateSymbolName(mangledName, demangledName_arr, demangledName_arr.size(), UNDNAME_COMPLETE);
		if (result != 0)
			return std::string(demangledName_arr.data());
		else return std::unexpected(GetLastError());

		#elif __has_include(<cxxabi.h>)
		
		// Code for the libstdc++ ABI
		int status;
		std::unique_ptr<const char[], decltype(std::free)> demangledName_c_str(
			abi::__cxa_demangle(typeid(callable).name(), nullptr, nullptr, &status),
			&std::free);
		if (status == 0)
		{
			std::string demangledName(demangledName_c_str);
			return demangledName;
		}
		else
		{
			return std::unexpected(status);
		}

		#else
		// This should not happen(!)
		return typeid(callable).name();
		#endif
	}

	template <typename... Args>
  constexpr std::enable_if_t<(std::is_convertible_v<Args, std::string> && ...), std::string>
  colonConcatenate(Args&&... args)
  {
		std::array<std::string, sizeof...(Args)> strings = { std::forward<Args>(args)... };
		std::string result;
		for (auto it = strings.begin(); it != strings.end(); it++)
		{
    	result += *it;
    	if (it != strings.end() - 1) result += ": ";
		}
		return result;
	}

	void consoleAndMsgBoxLog(const QString& msg, const QMessageLogContext& context, QtMsgType type = QtMsgType::QtInfoMsg, QWidget *parent = nullptr);
}
