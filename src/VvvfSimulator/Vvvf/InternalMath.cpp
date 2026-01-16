// Copyright © 2025 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Vvvf/InternalMath.cpp

#include "InternalMath.hpp"
// Packages
#include <QtMinMax>

namespace VvvfSimulator::Vvvf::InternalMath {
double triangle(double x) noexcept {
  double phase = m_2_PI * x - 4.0 * std::floor(x * m_1_2PI);
  if (1.0 <= phase && phase < 3)
    return 2.0 - phase;
  else if (3 <= phase)
    return phase - 4.0;
  else
    return phase;
}

double saw(double x) noexcept {
  double phase = m_1_PI * x - 2.0 * std::floor(x * m_1_2PI) - 1.0;
  return phase;
}

double square(double x) noexcept {
  double fixed_x = x - std::floor(x * m_1_2PI) * m_2PI;
  return fixed_x * m_1_PI > 1.0 ? -1.0 : 1.0;
}

namespace EquationSolver {
double NewtonMethod::operator()(double begin, double tolerance,
                                unsigned int n) {
  static const auto getDerivative = [this](double x) {
    double Fxdx = function(x + dx);
    double Fx = function(x);
    double Dy = Fxdx - Fx;
    return Dy / dx;
  };
  static const auto getZeroIntersect = [this](double x) {
    return -function(x) / getDerivative(x) + x;
  };

  double x = begin;
  for (unsigned int i = 0; i < n; i++) {
    double pre_x = x;
    x = getZeroIntersect(x);
    if (pre_x == x || std::isnan(x) || std::isinf(x))
      x = pre_x + dx;
    double fx = std::abs(function(x));
    if (fx < tolerance)
      return x;
  }
  return x;
}

double BisectionMethod::operator()(double x0, double x1, double tolerance,
                                   unsigned int n) {
  double xa = 0.0;
  for (unsigned int i = 0; i < n; i++) {
    xa = (x0 + x1) * 0.5;
    double ya = function(xa);
    if (function(x0) * ya < 0.0)
      x1 = xa;
    else
      x0 = xa;
    if (std::abs(ya) < tolerance)
      return xa;
  }
  return xa;
}
} // namespace EquationSolver

double lagrangePolynomial(double x, const std::span<QPointF> &points) {
  double y = 0.0, y_i;
  for (size_t i = 0; i < points.size(); i++) {
    y_i = 1.0;
    for (size_t j = 0; j < points.size(); j++)
      if (i != j)
        y_i *= (x - points[j].x()) / (points[i].x() - points[j].x());
    y += y_i * points[i].y();
  }
  return y;
}

QPointF norm(const QPointF &pt) {
  if (pt.isNull())
    return QPointF(0.0, 0.0);
  double len = 1.0 / std::sqrt(pt.x() * pt.x() + pt.y() * pt.y());
  return QPointF(pt.x() / len, pt.y() / len);
}

QPointF rotate(const QPointF &pt, double angle) {
  using namespace Functions;

  double cos = cosine(angle), sin = sine(angle);
  return QPointF(pt.x() * cos - pt.y() * sin, pt.x() * sin + pt.y() * cos);
}

QPointF max(const QPointF &a, const QPointF &b) {
  return QPointF(qMax(a.x(), b.x()), qMax(a.y(), b.y()));
}

QPointF min(const QPointF &a, const QPointF &b) {
  return QPointF(qMin(a.x(), b.x()), qMin(a.y(), b.y()));
}
} // namespace VvvfSimulator::Vvvf::InternalMath
