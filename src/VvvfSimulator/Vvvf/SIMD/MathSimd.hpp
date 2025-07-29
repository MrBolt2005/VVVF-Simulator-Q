#pragma once

#include "../InternalMath.hpp"

// Packages
#include <xsimd.hpp>
// Internal
#include "../../Util/Defines.h"
#include "../../Util/SIMD.hpp"

namespace NAMESPACE_VVVF::InternalMath::Functions
{
	namespace SIMD
	{	
		//
		// Functions::sineBatch
		//
		template <typename T, typename Arch = xsimd::default_arch>
		struct sineBatchDetail
		{
			QVector<T> operator()(const std::span<T> &x) const;
		};

		template <typename T, typename Arch>
		QVector<T> sineBatchDetail<T, Arch>::operator()(const std::span<T> &x) const
		{
			using namespace xsimd;
			using BType = batch<T, Arch>;

			QVector<T> rv(x.size(), Qt::Uninitialized);

			const size_t n = x.size() - (x.size() % BType::size);
			size_t i;
			for (i = 0; i < n; i += BType::size)
			{
				const auto xb = BType::load_unaligned(&(x[i])); // xb: x batch
				sin(xb).store_unaligned(&(rv[i]));
			}

			using namespace std;
			for (; i < x.size(); i++) rv[i] = sin(x[i]);
			return rv;
		}
	}
}
