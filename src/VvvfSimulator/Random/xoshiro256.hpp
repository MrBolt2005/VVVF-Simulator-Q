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

// xoshiro256ss.hpp
// Version 1.9.1.1

// Standard Library Includes
#include <array>
#include <cinttypes>

namespace VvvfSimulator::Random
{
	class xoshiro256ss
	{
		std::array<uint64_t, 4> s;
		uint64_t m_seed;

		static constexpr uint64_t splitmix64(uint64_t x)
		{
			uint64_t z = (x += 0x9e3779b97f4a7c15uLL);
			z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9uLL;
			z = (z ^ (z >> 27)) * 0x94d049bb133111ebuLL;
			return z ^ (z >> 31);
		}

		static constexpr uint64_t rotl(uint64_t x, int k)
		{
			return (x << k) | (x >> (64 - k));
		}
		
	public:
		static constexpr uint64_t defaultSeed = 0;
	
		constexpr xoshiro256ss() { *this = xoshiro256ss(defaultSeed); }
		constexpr xoshiro256ss(uint64_t Seed)
		{
			seed(Seed);
		}
		~xoshiro256ss() = default;

		constexpr uint64_t next() { return operator()(); }
		constexpr uint64_t operator()()
		{
			uint64_t result = rotl(s[1] * 5, 7) * 9;
			uint64_t t = s[1] << 17;
			s[2] ^= s[0];
			s[3] ^= s[1];
			s[1] ^= s[2];
			s[0] ^= s[3];
			s[2] ^= t;
			s[3] = rotl(s[3], 45);
			return result;
	}
		constexpr void seed(uint64_t seed)
		{
			m_seed = seed;
			const uint64_t stateValue = splitmix64(seed);
			for (auto& state : s)
				state = stateValue;
		}
	};
}