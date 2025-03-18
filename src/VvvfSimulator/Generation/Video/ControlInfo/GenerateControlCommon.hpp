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

// GenerateControlCommon.hpp
// Version 1.9.1.1

// Standard Library
#include <array>
#include <utility>
// Packages
#include <QBrush>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QFontMetricsF>
#include <QPainter>
#include <QPen>
#include <QPoint>
#include <QSize>
#include <QString>

namespace VvvfSimulator::Generation::Video::ControlInfo::GenerateControlCommon
{
	void filledCornerCurvedRectangle(QPainter &painter, const QBrush &brush, const QPoint &start, const QPoint &end, int roundRadius);

	inline QSize measureString(const QString &text, const QFont &font)
	{
		const QFontMetrics metrics(font);
		return metrics.size(Qt::TextSingleLine, text);
	}

	inline QSizeF measureStringF(const QString &text, const QFont &font)
	{
		const QFontMetricsF metrics(font);
		return metrics.size(Qt::TextSingleLine, text);
	}

	QPoint centerTextWithFilledCornerCurvedRectangle(
		QPainter &painter,
		const QString &str,
		const QBrush &strBr,
		const QFont &fnt,
		const QBrush &br,
		const QPoint &start,
		const QPoint &end,
		int roundRadius,
		const QPoint &strCompen);
	
	void lineCornerCurvedRectangle(QPainter &painter, const QPen &pen, const QPoint &start, const QPoint &end, int roundRadius);

	void titleStrWithLineCornerCurvedRectangle(
		QPainter &painter,
		const QString &str,
		const QBrush &strBr,
		const QFont &fnt,
		const QPen &pen,
		const QPoint &start,
		const QPoint &end,
		int roundRadius,
		const QPoint &strCompen
	);
	
	QPoint centerTextWithLineCornerCurvedRectangle(
		QPainter &painter,
		const QString &str,
		const QBrush &strBr,
		const QFont &fnt,
		const QPen &pen,
		const QPoint &start,
		const QPoint &end,
		int roundRadius,
		const QPoint &strCompen
	);

	inline Qt::AlignmentFlag getAlignment(const QString &family)
	{
		constexpr std::array<QFontDatabase::WritingSystem, 4> RTLWritingSystems
			{QFontDatabase::Arabic, QFontDatabase::Hebrew, QFontDatabase::Syriac, QFontDatabase::Thaana};
		const auto writingSystems = QFontDatabase::writingSystems(family);
    for (const auto &ws : writingSystems)
			if (std::find(RTLWritingSystems.begin(), RTLWritingSystems.end(), ws) != RTLWritingSystems.end())
				return Qt::AlignRight;
    return Qt::AlignLeft;
	}

	inline Qt::AlignmentFlag getAlignment(const QFont &font)
	{
		return getAlignment(font.family());
	}
}