#pragma once

// Standard Library
#include <typeindex>
// Packages
#include <boost/preprocessor.hpp>
#include <QByteArrayView>
#include <QList>
#include <QPair>
#include <QSet>
#include <QVariant>
#include <QtProcessorDetection>
#include <xsimd.hpp>
// Internal
#include "String.hpp"

namespace VvvfSimulator::Util::SIMD
{
	//
	// Basic Configurations
	//

	#define SIMD_REMOVE_PARENS_OP(s, data, elem) BOOST_PP_REMOVE_PARENS(elem)
	#define SIMD_PP_SEQ_TO_COMMA_LIST(seq) \
		BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(SIMD_REMOVE_PARENS_OP, _, seq))
	#define SIMD_MAKE_ARCH_LIST(seq) xsimd::arch_list<SIMD_PP_SEQ_TO_COMMA_LIST(seq)>
	
	// SIMD architectures supported per platform(s):
	// - x86-64
	#define SIMD_X86_64_ARCHITECTURES \
		(xsimd::generic) \
		(xsimd::sse2) \
		(xsimd::sse3) \
		(xsimd::ssse3) \
		(xsimd::sse4_1) \
		(xsimd::sse4_2) \
		(xsimd::avx) \
		(xsimd::avx2) \
		(xsimd::fma3) \
		(xsimd::fma4) \
		(xsimd::avx512f) \
		(xsimd::avx512bw) \
		(xsimd::avx512dq) \
		(xsimd::avx512cd) \
		(xsimd::avx512vl) \
		(xsimd::avx512vnni) \
		(xsimd::avx512bf16) \
		(xsimd::avx512vbmi) \
		(xsimd::avx512vbmi2) \
		(xsimd::avx512ifma) \
		(xsimd::avx512vpopcntdq)
	#define SIMD_X86_64_ARCH_LIST SIMD_MAKE_ARCH_LIST(SIMD_X86_64_ARCHITECTURES)
	// - x86 (IA-32)
	#define SIMD_X86_32_ARCHITECTURES \
		(xsimd::generic) \
		(xsimd::sse2) \
		(xsimd::sse3) \
		(xsimd::ssse3) \
		(xsimd::sse4_1) \
		(xsimd::sse4_2)
	#define SIMD_X86_32_ARCH_LIST SIMD_MAKE_ARCH_LIST(SIMD_X86_32_ARCHITECTURES)
	// - ARM64/AArch64
	#define SIMD_ARM64_ARCHITECTURES \
		(xsimd::generic) \
		(xsimd::neon) \
		(xsimd::neon64) \
		(xsimd::sve)
	#define SIMD_ARM64_ARCH_LIST SIMD_MAKE_ARCH_LIST(SIMD_ARM64_ARCHITECTURES)
	// - ARM32/AArch32
	#define SIMD_ARM32_ARCHITECTURES \
		(xsimd::generic) \
		(xsimd::neon) /*\
		(xsimd::sve) */
	#define SIMD_ARM32_ARCH_LIST SIMD_MAKE_ARCH_LIST(SIMD_ARM32_ARCHITECTURES)
	// WebAssembly (WASM)
	#define SIMD_WASM_ARCHITECTURES \
		(xsimd::generic) \
		(xsimd::wasm)
	#define SIMD_WASM_ARCH_LIST SIMD_MAKE_ARCH_LIST(SIMD_WASM_ARCHITECTURES)
	// xsimd won't support other ones as for now
	#define SIMD_GENERIC_ARCHITECTURES \
		(xsimd::generic)
	#define SIMD_GENERIC_ARCH_LIST SIMD_MAKE_ARCH_LIST(SIMD_GENERIC_ARCHITECTURES)

	// Default architecture list detection
	#if   defined(Q_PROCESSOR_X86_64)
		#define SIMD_DEF_ARCHITECTURES SIMD_X86_64_ARCHITECTURES
		#define SIMD_DEF_ARCH_LIST SIMD_X86_64_ARCH_LIST
		#define SIMD_DEF_ARCH_STRING "x86-64"
	#elif defined(Q_PROCESSOR_X86) // /Q_PROCESSOR_X86_32
		#define SIMD_DEF_ARCHITECTURES SIMD_x86_32_ARCHITECTURES
		#define SIMD_DEF_ARCH_LIST SIMD_x86_32_ARCH_LIST
		#define SIMD_DEF_ARCH_STRING "x86-32"
	#elif defined(Q_PROCESSOR_ARM_64)
		#define SIMD_DEF_ARCHITECTURES SIMD_ARM64_ARCHITECTURES
		#define SIMD_DEF_ARCH_LIST SIMD_ARM64_ARCH_LIST
		#define SIMD_DEF_ARCH_STRING "arm64"
	#elif defined(Q_PROCESSOR_ARM_32)
		#define SIMD_DEF_ARCHITECTURES SIMD_ARM32_ARCHITECTURES
		#define SIMD_DEF_ARCH_LIST SIMD_ARM32_ARCH_LIST
		#define SIMD_DEF_ARCH_STRING "arm32"
	#elif defined(Q_PROCESSOR_WASM)
		#define SIMD_DEF_ARCHITECTURES SIMD_WASM_ARCHITECTURES
		#define SIMD_DEF_ARCH_LIST SIMD_WASM_ARCH_LIST
		#define SIMD_DEF_ARCH_STRING "wasm"
	#else
		#define SIMD_DEF_ARCHITECTURES SIMD_GENERIC_ARCHITECTURES
		#define SIMD_DEF_ARCH_LIST SIMD_GENERIC_ARCH_LIST
		#define SIMD_DEF_ARCH_STRING "Generic"
	#endif

	namespace Debug
	{
		struct PlatformConfiguration
		{
			QByteArrayView isaFamily{}, name{};
			QSet<std::type_index> recommendedTypes{};
			qint64 cacheLineSize{};
			qint64 maxVectorSize{};
			qint64 preferredAlignment{};
			bool hasVariableLength{};

			PlatformConfiguration()  noexcept = default;
			PlatformConfiguration(const QByteArrayView &family, const QByteArrayView &name,
				const QSet<std::type_index> &types, qint64 cacheLine, qint64 maxVector,
				qint64 alignment, bool variableLength)
				: isaFamily(family), recommendedTypes(types), cacheLineSize(cacheLine),
				  maxVectorSize(maxVector), preferredAlignment(alignment), hasVariableLength(variableLength)
			{}
			~PlatformConfiguration() = default;

			bool isGeneric() const noexcept
			{
				return isaFamily == "generic";
			}

			/// Returns a list of platform information.
			/// This is used for debugging purposes.
			QList<QPair<QByteArrayView, QVariant>> information();
		};

		extern const PlatformConfiguration  PlatformConfig_x86_64;
		extern const PlatformConfiguration  PlatformConfig_x86_32;
		extern const PlatformConfiguration  PlatformConfig_ARM64;
		extern const PlatformConfiguration  PlatformConfig_ARM32;
		extern const PlatformConfiguration  PlatformConfig_WASM;
		extern const PlatformConfiguration  PlatformConfig_Generic;
		extern const PlatformConfiguration *PlatformConfig_Default;
	}

	//
	// Automatic runtime dispatch
	//
	template <typename Function, typename... Args>
	inline auto runtimeDispatch(Function &&func, Args &&...args)
	{
		return xsimd::dispatch<SIMD_DEF_ARCH_LIST>(
			std::forward<Function>(func),
			std::forward<Args>(args)...
		);
	}
}