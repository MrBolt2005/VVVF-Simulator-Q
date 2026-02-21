#include "InternalMath.hpp"

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Vvvf/InternalMathSimd.cpp
 * v1.10.0.1
 */

// Highway (SIMD)
#undef HWY_TARGET_INCLUDE
#define HWY_TARGET_INCLUDE "InternalMathSimd.cpp" // this file
#include <hwy/foreach_target.h> // must come before highway.h
#include <hwy/highway.h>
#include <hwy/contrib/math/math-inl.h>

// Standard Library
#include <cmath>

// Common Highway dispatching code

namespace VvvfSimulator::Vvvf::InternalMath {
HWY_BEFORE_NAMESPACE();

namespace HWY_NAMESPACE {
namespace hn = hwy::HWY_NAMESPACE;

template <class V>
V fmodVec(V x, V y) {
    const auto q = hn::Trunc(hn::Div(x, y));
    return hn::NegMulAdd(q, y, x); // Equivalent to x - (q * y)
}

template <typename T>
/*
 * Explicitly marking inline because otherwise the compiler could outline the
 * templated version and make the non-templated counterpart a jump to it,
 * adding a layer of indirection:
 */
inline void sineVecDetail(T *HWY_RESTRICT out, const T *HWY_RESTRICT in,
                          std::size_t size) {
    const hn::ScalableTag<T> d;
    std::size_t i;
    // 6207*2*pi, smallest integer product of 2*pi before 39,000
    constexpr auto divisorK = T(38999.7312016636932623);
    const auto divisor = hn::Set(d, divisorK);
    for (i = 0; i < size; i += hn::Lanes(d)) {
        auto ins = hn::Load(d, in + i);
        // Reflect inputs
        ins = fmodVec(ins, divisor);
        const auto results = hn::Sin(d, ins);
        hn::Store(results, d, out + i);
    }
    if (i > size) {
        for (i = i - hn::Lanes(d); i < size; i++) {
            out[i] = std::sin(in[i]);
        }
    }
}

void sineVecDetailF32(float *HWY_RESTRICT out, const float *HWY_RESTRICT in,
                      std::size_t size) {
    return sineVecDetail<float>(out, in, size);
}
}; // namespace HWY_NAMESPACE

HWY_AFTER_NAMESPACE();
} // namespace VvvfSimulator::Vvvf::InternalMath

// API code

#if HWY_ONCE
namespace VvvfSimulator::Vvvf::InternalMath {
// Declare the dynamic dispatch array
HWY_EXPORT(sineVecDetailF32);

void sineVec(float *HWY_RESTRICT out, const float *HWY_RESTRICT in,
             std::size_t size) {
    return HWY_DYNAMIC_DISPATCH(sineVecDetailF32)(out, in, size);
}
} // namespace VvvfSimulator::Vvvf::InternalMath
#endif // HWY_ONCE