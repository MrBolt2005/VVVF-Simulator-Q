#pragma once

#include "../InternalMath.hpp"

// Packages
#include <xsimd.hpp>
// Internal
#include "../../Util/Defines.h"

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
			QVector<T> operator()(const std::span<T> &x) const
			QVector<T> sineBatchDetail<T, Arch>::operator()(const std::span<T> &x) const
			{
				using namespace xsimd;
				using BType = batch<T, Arch>;

				QVector<T> rv(x.size(), Qt::Uninitialized);

				const size_t n = x.size() - (x.size() % BType::size), i;
				for (i = 0; i < n; i += BType::size)
				{
					const auto xb = BType::load_unaligned(&(x[i])); // xb: x batch
					const auto yb = sin(xb);
					yb.store_unaligned(&(rv[i]));
				}

				using namespace std;
				for (; i < x.size(); i++) rv[i] = sin(x[i]);
				return rv;
			}
		};

		#define SPECIALIZE_SINEBATCHDETAIL(T, Arch) \
		template <> \
		struct sineBatchDetail<T, Arch> \
		{ \
    	QVector<T> operator()(const std::span<T> &x) const; \
    };
		#define INSTANTIATE_SINEBATCHDETAIL(T, Arch) \
		template struct sineBatchDetail<T, Arch>;
		#define EXTERN_SINEBATCHDETAIL(T, Arch) \
		extern INSTANTIATE_SINEBATCHDETAIL(T, Arch)
	}
}