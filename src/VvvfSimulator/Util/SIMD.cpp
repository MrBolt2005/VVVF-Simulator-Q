#include "SIMD.hpp"

// Standard Library
#include <set>

namespace VvvfSimulator::Util::SIMD
{
	namespace Debug
	{
		QList<QPair<QByteArrayView, QVariant>> PlatformConfiguration::information()
		{
			// === XSIMD Platform Information ===
			QList<QPair<QByteArrayView, QVariant>> info;
			info.reserve(7);

			info.append({ "xsimd Support", !isGeneric()});
			info.append({ "ISA Family", QVariant::fromValue(isaFamily) });
			info.append({ "Cache Line Size", cacheLineSize });
			info.append({ "Maximum Vector Size", maxVectorSize });
			info.append({ "Preferred Alignment", preferredAlignment });
			info.append({ "Variable Length Support", hasVariableLength });

			// Extract type names from recommendedTypes
			std::set<QByteArrayView> typeNames;
			for (const auto &type : recommendedTypes)
				typeNames.emplace(type.name());

			info.append({ "Recommended Supported Types", QVariant::fromValue(typeNames) });
			return info;
		}

		const PlatformConfiguration PlatformConfig_x86_64 =
		{
			"x86",
			"x86-64",
			{
				typeid(float), typeid(double), typeid(int8_t), typeid(uint8_t),
				typeid(int16_t), typeid(uint16_t), typeid(int32_t),
				typeid(uint32_t), typeid(int64_t), typeid(uint64_t)
			},
			64,   // Cache line size
			64,   // Maximum vector size - AVX-512
			32,   // Preferred alignment - AVX
			false // Variable vector length support
		};

		const PlatformConfiguration PlatformConfig_x86_32 =
		{
			"x86",
			"x86 (IA-32)",
			{
				typeid(float), typeid(double), typeid(int8_t), typeid(uint8_t),
				typeid(int16_t), typeid(uint16_t), typeid(int32_t),
				typeid(uint32_t), typeid(int64_t), typeid(uint64_t)
			},
			64,   // Cache line size
			16,   // Maximum vector size - SSE
			16,   // Preferred alignment - SSE
			false // Variable vector length support
		};

		const PlatformConfiguration PlatformConfig_ARM64 =
		{
			"ARM",
			"ARM (AArch64)",
			{
				typeid(float), typeid(double), typeid(int8_t), typeid(uint8_t),
				typeid(int16_t), typeid(uint16_t), typeid(int32_t), typeid(uint32_t)
			},
			64,   // Cache line size
			256,  // Maximum vector size - SVE
			16,   // Preferred alignment - NEON
			false // Variable vector length support
		};

		const PlatformConfiguration PlatformConfig_ARM32 =
		{
			"ARM",
			"ARM (32-bit)",
			{
				typeid(float), typeid(double), typeid(int8_t), typeid(uint8_t),
				typeid(int16_t), typeid(uint16_t), typeid(int32_t), typeid(uint32_t)
			},
			64,   // Cache line size
			16,   // Maximum vector size - NEON
			16,   // Preferred alignment - NEON
			false // Variable vector length support
		};

		const PlatformConfiguration PlatformConfig_WASM =
		{
			"WASM",
			"WebAssembly (WASM)",
			{
				typeid(float), typeid(double), typeid(int8_t), typeid(uint8_t),
				typeid(int16_t), typeid(uint16_t), typeid(int32_t), typeid(uint32_t)
			},
			64,   // Cache line size
			16,   // Maximum vector size - WASM SIMD (128-bit)
			16,   // Preferred alignment - WASM SIMD (128-bit)
			false // Variable vector length support
		};

		const PlatformConfiguration PlatformConfig_Generic =
		{
			"Generic",
			"Generic SIMD architecture",
			{},
			-1,   // Cache line size
			-1,   // Maximum vector size - Generic
			-1,   // Preferred alignment - Generic
			false // Variable vector length support
		};

		#if   defined(Q_PROCESSOR_X86_64)
			const PlatformConfiguration *PlatformConfig_Default = &PlatformConfig_x86_64;
		#elif defined(Q_PROCESSOR_X86) // /Q_PROCESSOR_X86_32
			const PlatformConfiguration *PlatformConfig_Default = &PlatformConfig_x86_32;
		#elif defined(Q_PROCESSOR_ARM_64)
			const PlatformConfiguration *PlatformConfig_Default = &PlatformConfig_ARM64;
		#elif defined(Q_PROCESSOR_ARM_32)
			const PlatformConfiguration *PlatformConfig_Default = &PlatformConfig_ARM32;
		#elif defined(Q_PROCESSOR_WASM)
			const PlatformConfiguration *PlatformConfig_Default = &PlatformConfig_WASM;
		#else
			const PlatformConfiguration *PlatformConfig_Default = &PlatformConfig_Generic;
			#pragma message("Limited SIMD support: only generic operations available")
		#endif
	}
}