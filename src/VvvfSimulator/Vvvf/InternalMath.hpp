#pragma once

// Copyright © 2025 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Vvvf/InternalMath.hpp

// Standard Library
#include <cmath>
#include <functional>
#include <span>
// Packages
#include <QPointF>
#include <hwy/base.h>
// Internal
// #include "Namespace_VVVF.h"

namespace VvvfSimulator::Vvvf::InternalMath {
constexpr double m_2_PI = 0.63661977236758134307553505349006;
constexpr double m_1_PI = 0.31830988618379067153776752674503;
constexpr double m_1_2PI = 0.15915494309189533576888376337251;

constexpr double m_4PI_3 = 4.1887902047863909846168578443727;

constexpr double m_2PI = 6.283185307179586476925286766559;
constexpr double m_2PI_3 = 2.0943951023931954923084289221863;

constexpr double m_PI = 3.1415926535897932384626433832795;
constexpr double m_PI_2 = 1.5707963267948966192313216916398;
constexpr double m_PI_3 = 1.0471975511965977461542144610932;
constexpr double m_PI_4 = 0.78539816339744830961566084581988;
constexpr double m_PI_6 = 0.52359877559829887307710723054658;
constexpr double m_PI_12 = 0.26179938779914943653855361527329;
constexpr double m_PI_180 = 0.01745329251994329576923690768489;

constexpr double m_SQRT3 = 1.7320508075688772935274463415059;
constexpr double m_SQRT3_2 = 0.86602540378443864676372317075294;

namespace Functions {
// Scalar operations
double triangle(double x) noexcept;
double saw(double x) noexcept;
inline auto sine(auto x) {
    return std::sin(x);
}
inline auto cosine(auto x) {
    return std::cos(x);
}
inline auto arcSine(auto x) {
    return std::asin(x);
}
double square(double x) noexcept;

// Batched operations
/**
 * @brief Calculates the sine of real numbers in batch, such that
 * out[idx] = sine(in[idx]), using SIMD operations whenever available/possible.
 * @param out The output array.
 * @param in The input array.
 * @param size The size of both arrays to process.
 * @returns Nothing. The function reads from the parameter in and writes to the
 * parameter out.
 * @throws Should throw nothing.
 */
void sineVec(float *HWY_RESTRICT out, const float *HWY_RESTRICT in,
             std::size_t size);
} // namespace Functions

namespace EquationSolver {
typedef std::function<double(double)> Function;

struct NewtonMethod {
    Function function;
    double dx{};

    NewtonMethod() = default;
    NewtonMethod(const Function &func, double Dx)
        : function(func)
        , dx(Dx) {}
    NewtonMethod(Function &&func, double Dx)
        : function(func)
        , dx(Dx) {}

    double operator()(double begin, double tolerance, unsigned int n);
};

struct BisectionMethod {
    Function function;

    BisectionMethod() = default;
    BisectionMethod(const Function &func)
        : function(func) {}
    BisectionMethod(Function &&func)
        : function(func) {}

    double operator()(double x0, double x1, double tolerance, unsigned int n);
};
} // namespace EquationSolver

double lagrangePolynomial(double x, const std::span<QPointF> &points);

QPointF norm(const QPointF &pt);
QPointF rotate(const QPointF &pt, double angle);
QPointF max(const QPointF &a, const QPointF &b);
QPointF min(const QPointF &a, const QPointF &b);
} // namespace VvvfSimulator::Vvvf::InternalMath