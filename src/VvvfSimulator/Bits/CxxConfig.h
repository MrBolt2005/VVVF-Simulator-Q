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

// CxxConfig.h
// Version 1.9.1.1

#if __cplusplus >= 202302L
	#define CXX23_CONSTEXPR constexpr
	#define CXX23_NOEXCEPT  noexcept
	#define CXX23_INLINE    inline
#else
	#define CXX23_CONSTEXPR
	#define CXX23_NOEXCEPT
	#define CXX23_INLINE
#endif

#if __cplusplus >= 202002L
	#define CXX20_CONSTEXPR   constexpr
	#define CXX20_NOEXCEPT    noexcept
	#define CXX20_INLINE      inline
	#define CONSTEXPR_VIRTUAL constexpr virtual
#else
	#define CXX20_CONSTEXPR
	#define CXX20_NOEXCEPT
	#define CXX20_INLINE
	#define CONSTEXPR_VIRTUAL virtual
#endif

#if __cplusplus >= 201703L
	#define CXX17_CONSTEXPR   constexpr
	#define CXX17_NOEXCEPT    noexcept
	#define CXX17_INLINE      inline
#else
	#define CXX17_CONSTEXPR
	#define CXX17_NOEXCEPT
	#define CXX17_INLINE
#endif
// Compare this snippet from src/VvvfSimulator/Bits/CxxConfig.h: