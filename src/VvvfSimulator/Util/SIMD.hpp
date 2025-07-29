#pragma once

// Standard Library
#include <tuple>
#include <typeindex>
#include <utility>
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

	// SIMD architectures supported per platform(s):
	// - x86-64
	using x86_64_ArchList = xsimd::arch_list<
    ::xsimd::generic,
    ::xsimd::sse2,
    ::xsimd::sse3,
    ::xsimd::ssse3,
    ::xsimd::sse4_1,
    ::xsimd::sse4_2,
    ::xsimd::avx,
    ::xsimd::avx2,
    // ::xsimd::fma3,
    ::xsimd::fma4,
    ::xsimd::avx512f,
    ::xsimd::avx512bw,
    ::xsimd::avx512dq,
    ::xsimd::avx512cd,
    // ::xsimd::avx512vnni,
    ::xsimd::avx512vbmi,
    ::xsimd::avx512ifma
>;
	#define SIMD_X86_64_ARCH_STRING "x86-64"
	// - x86 (IA-32)
	using x86_32_ArchList = xsimd::arch_list<
		::xsimd::generic,
		::xsimd::sse2,
		::xsimd::sse3,
		::xsimd::ssse3,
		::xsimd::sse4_1,
		::xsimd::sse4_2
	>;
	#define SIMD_X86_32_ARCH_STRING "x86-32"
	// - ARM64/AArch64
	using Arm64_ArchList = xsimd::arch_list<
		::xsimd::generic,
		::xsimd::neon,
		::xsimd::neon64,
		::xsimd::sve
	>;
	#define SIMD_ARM64_ARCH_STRING "arm64"
	// - ARM32/AArch32
	using Arm32_ArchList = xsimd::arch_list<
		::xsimd::generic,
		::xsimd::neon //,
		// xsimd::sve
	>;
	#define SIMD_ARM32_ARCH_STRING "arm32"
	// WebAssembly (WASM)
	using WASM_ArchList = xsimd::arch_list<
		::xsimd::generic,
		::xsimd::wasm
	>;
	#define SIMD_WASM_ARCH_STRING "wasm"
	// xsimd won't support other ones as for now
	using Generic_ArchList = xsimd::arch_list<::xsimd::generic>;
	#define SIMD_GENERIC_ARCH_STRING "Generic"

	// Default architecture list detection
	#if   defined(Q_PROCESSOR_X86_64)
		using DefaultArchList = x86_64_ArchList;
		#define SIMD_DEF_ARCH_STRING SIMD_X86_64_ARCH_STRING
	#elif defined(Q_PROCESSOR_X86) // /Q_PROCESSOR_X86_32
		using DefaultArchList = x86_32_ArchList;
		#define SIMD_DEF_ARCH_STRING SIMD_X86_32_ARCH_STRING
	#elif defined(Q_PROCESSOR_ARM_64)
		using DefaultArchList = Arm64_ArchList;
		#define SIMD_DEF_ARCH_STRING SIMD_ARM64_ARCH_STRING
	#elif defined(Q_PROCESSOR_ARM_32)
		using DefaultArchList = Arm32_ArchList;
		#define SIMD_DEF_ARCH_STRING SIMD_ARM32_ARCH_STRING
	#elif defined(Q_PROCESSOR_WASM)
		using DefaultArchList = WASM_ArchList;
		#define SIMD_DEF_ARCH_STRING SIMD_WASM_ARCH_STRING
	#else
		using DefaultArchList = Generic_ArchList;
		#define SIMD_DEF_ARCH_STRING SIMD_GENERIC_ARCH_STRING
	#endif

	namespace Debug
	{
		struct PlatformConfiguration
		{
			QByteArray isaFamily{}, longName{}, shortName{};
			QSet<std::type_index> recommendedTypes{};
			qint64 cacheLineSize{};
			qint64 maxVectorSize{};
			qint64 preferredAlignment{};
			bool hasVariableLength{};

			PlatformConfiguration()  noexcept = default;
			PlatformConfiguration(
				const QByteArray &family,
				const QByteArray &lName,
				const QByteArray &sName,
				const QSet<std::type_index> &types,
				qint64 cacheLine,
				qint64 maxVector,
				qint64 alignment,
				bool variableLength
			)
				: isaFamily(family)
				, longName(lName)
				, shortName(sName)
				, recommendedTypes(types) 
				, cacheLineSize(cacheLine) 
				, maxVectorSize(maxVector)
				, preferredAlignment(alignment)
				, hasVariableLength(variableLength)
			{}
			~PlatformConfiguration() = default;

			bool isGeneric() const noexcept
			{
				return isaFamily == SIMD_GENERIC_ARCH_STRING;
			}

			/// Returns a list of platform information.
			/// This is used for debugging purposes.
			QList<QPair<QByteArrayView, QVariant>> information();
		};

		extern const PlatformConfiguration        PlatformConfig_x86_64;
		extern const PlatformConfiguration        PlatformConfig_x86_32;
		extern const PlatformConfiguration        PlatformConfig_ARM64;
		extern const PlatformConfiguration        PlatformConfig_ARM32;
		extern const PlatformConfiguration        PlatformConfig_WASM;
		extern const PlatformConfiguration        PlatformConfig_Generic;
		extern const PlatformConfiguration *const PlatformConfig_Default;
	}

	//
	// Automatic runtime dispatch
	//
	template <typename Function, typename... Args>
	inline auto runtimeDispatch(Function &&func, Args &&...args)
	{
		return xsimd::dispatch<DefaultArchList>(
			std::forward<Function>(func),
			std::forward<Args>(args)...
		);
	}

	//
	// Automatic runtime dispatch: Extern & instantiation helpers
	//

	template <
		template <typename, typename> class TemplateName,
		typename T, typename Arch = xsimd::default_arch
	>
	void externTemplate()
	{
		extern template struct TemplateName<T, Arch>;
	}

	template <
		template <typename, typename> class TemplateName,
		typename T, typename Arch = xsimd::default_arch
	>
	void instantiateTemplate()
	{
		template struct TemplateName<T, Arch>;
	}

	// Helper to pack
	template <
		template<typename, typename> class TemplateName,
		typename... Types, typename... Archs
	>
	void forEachTypeArchExtern(std::tuple<Types...>, xsimd::arch_list<Archs...>)
	{
		(void)std::initializer_list<int>{
			( (void)externTemplate<TemplateName, Types, Archs>(), 0 )...
		};
	}

	template <
		template<typename, typename> class TemplateName,
		typename... Types, typename... Archs
	>
	void forEachTypeArchInstantiate(std::tuple<Types...>, xsimd::arch_list<Archs...>)
	{
		(void)std::initializer_list<int>{
			( (void)instantiateTemplate<TemplateName, Types, Archs>(), 0 )...
		};
	}

	#define SIMD_EXTERN_FOR_LISTS(TemplateName, ArchList, TypeList) \
		forEachTypeArchExtern<TemplateName>(TypeList{}, ArchList{});

	#define SIMD_INSTANTIATE_FOR_LISTS(TemplateName, ArchList, TypeList) \
		forEachTypeArchInstantiate<TemplateName>(TypeList{}, ArchList{});
}