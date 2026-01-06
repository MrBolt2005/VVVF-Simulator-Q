#pragma once

// Standard Library
#include <array>
// Internal
//#include "../../QtVideoWriter.hpp"
// Packages
#include <QColor>
#include <QImage>
#include <QVector>

namespace VvvfSimulator::Generation::Video::FS::GenerateFourierSeries
{
	namespace MagnitudeColor
	{
		constexpr double linear(double x, double x1, double x2, double y1, double y2)
		{
			double val = (y2 - y1) / (x2 - x1) * (x - x1) + y1;
			return val;
		}

		constexpr std::array<double, 3> linearRGB(double x, double x1, double x2, double r1, double g1, double b1, double r2, double g2, double b2)
		{
			return { linear(x, x1, x2, r1, r2), linear(x, x1, x2, g1, g2), linear(x, x1, x2, b1, b2) };
		}

		constexpr QColor getColor(double rate)
		{
			std::array<double, 3> rgb = {0, 0, 0};
			if (rate >= 0.85) rgb = {255, 85, 85};
			if (rate < 0.85) rgb = linearRGB(rate, 0.85, 0.68, 255, 85, 85, 255, 205, 85);
			if (rate < 0.68) rgb = linearRGB(rate, 0.68, 0.5, 255, 205, 85, 206, 224, 0);
			if (rate < 0.5) rgb = linearRGB(rate, 0.5, 0.38, 206, 224, 0, 115, 227, 117);
			if (rate < 0.38) rgb = linearRGB(rate, 0.15, 0.38, 77, 148, 232, 115, 227, 117);
			if (rate < 0.15) rgb = {77, 148, 232};

			return QColor::fromRgb(static_cast<int>(rgb[0]), static_cast<int>(rgb[1]), static_cast<int>(rgb[2]));
		}
	}

	QImage getImage(const QVector<double>& coefficients, int width = 1000, int height = 1000, bool darkMode = false);
}