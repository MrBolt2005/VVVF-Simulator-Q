#include "GenerateControlCommon.hpp"

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

// GenerateControlCommon.cpp
// Version 1.9.1.1

namespace VvvfSimulator::Generation::Video::ControlInfo::GenerateControlCommon
{
	void filledCornerCurvedRectangle(QPainter &painter, const QBrush &brush, const QPoint &start, const QPoint &end, int roundRadius)
	{
    const int width = end.x() - start.x();
    const int height = end.y() - start.y();

    painter.setBrush(brush);

    // Draw the rectangles
    painter.drawRect(start.x() + roundRadius, start.y(), width - 2 * roundRadius, height);
    painter.drawRect(start.x(), start.y() + roundRadius, roundRadius, height - 2 * roundRadius);
    painter.drawRect(end.x() - roundRadius, start.y() + roundRadius, roundRadius, height - 2 * roundRadius);

    // Draw the ellipses (corners)
    painter.drawEllipse(start.x(), start.y(), roundRadius * 2, roundRadius * 2);
    painter.drawEllipse(start.x(), start.y() + height - roundRadius * 2, roundRadius * 2, roundRadius * 2);
    painter.drawEllipse(start.x() + width - roundRadius * 2, start.y(), roundRadius * 2, roundRadius * 2);
    painter.drawEllipse(start.x() + width - roundRadius * 2, start.y() + height - roundRadius * 2, roundRadius * 2, roundRadius * 2);
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
		const QPoint &strCompen
  )
	{
		const QSize strSize = measureString(str, fnt);

		const int width = end.x() - start.x();
    const int height = end.y() - start.y();

    filledCornerCurvedRectangle(painter, br, start, end, roundRadius);

    const QPoint strPos(start.x() + width  / 2 - strSize.width()  / 2 + strCompen.x(),
		                    start.y() + height / 2 - strSize.height() / 2 + strCompen.y());

    painter.setFont(fnt);
    painter.setBrush(strBr);
    painter.drawText(strPos, str);

    return strPos;
	}

	void lineCornerCurvedRectangle(QPainter &painter, const QPen &pen, const QPoint &start, const QPoint &end, int roundRadius)
	{
		const int width = end.x() - start.x();
    const int height = end.y() - start.y();

    painter.setPen(pen);

    // Draw the lines
    painter.drawLine(start.x() + roundRadius, start.y(), end.x() - roundRadius + 1, start.y());
    painter.drawLine(start.x() + roundRadius, end.y(), end.x() - roundRadius + 1, end.y());
    painter.drawLine(start.x(), start.y() + roundRadius, start.x(), end.y() - roundRadius + 1);
    painter.drawLine(end.x(), start.y() + roundRadius, end.x(), end.y() - roundRadius + 1);

    // Draw the arcs (corners)
    painter.drawArc(start.x(), start.y(), roundRadius * 2, roundRadius * 2, 90 * 16, 90 * 16);
    painter.drawArc(start.x(), start.y() + height - roundRadius * 2, roundRadius * 2, roundRadius * 2, 180 * 16, 90 * 16);
    painter.drawArc(start.x() + width - roundRadius * 2, start.y(), roundRadius * 2, roundRadius * 2, 0, 90 * 16);
    painter.drawArc(start.x() + width - roundRadius * 2, start.y() + height - roundRadius * 2, roundRadius * 2, roundRadius * 2, 270 * 16, 90 * 16);
	}

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
  )
	{
		// Measure String
		const QFontMetrics metrics(fnt);
    const QSize strSize = metrics.size(Qt::TextSingleLine, str);

    const int width = end.x() - start.x();
    const int height = end.y() - start.y();

    painter.setPen(pen);
    painter.setFont(fnt);
    painter.setBrush(strBr);

    // Draw the lines
    painter.drawLine(start.x() + roundRadius, start.y(), start.x() + width / 2 - strSize.width() / 2 - 10, start.y());
    painter.drawLine(end.x() - roundRadius + 1, start.y(), start.x() + width / 2 + strSize.width() / 2 + 10, start.y());

    // Draw the text
    painter.drawText(start.x() + width / 2 - strSize.width() / 2 + strCompen.x(), start.y() - metrics.height() / 2 + strCompen.y(), str);

    painter.drawLine(start.x() + roundRadius, end.y(), end.x() - roundRadius + 1, end.y());
    painter.drawLine(start.x(), start.y() + roundRadius, start.x(), end.y() - roundRadius + 1);
    painter.drawLine(end.x(), start.y() + roundRadius, end.x(), end.y() - roundRadius + 1);

    // Draw the arcs (corners)
    painter.drawArc(start.x(), start.y(), roundRadius * 2, roundRadius * 2, 90 * 16, 90 * 16);
    painter.drawArc(start.x(), start.y() + height - roundRadius * 2, roundRadius * 2, roundRadius * 2, 180 * 16, 90 * 16);
    painter.drawArc(start.x() + width - roundRadius * 2, start.y(), roundRadius * 2, roundRadius * 2, 0, 90 * 16);
    painter.drawArc(start.x() + width - roundRadius * 2, start.y() + height - roundRadius * 2, roundRadius * 2, roundRadius * 2, 270 * 16, 90 * 16);
	}

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
  )
	{
		const QSize strSize = measureString(str, fnt);

    const int width = end.x() - start.x();
    const int height = end.y() - start.y();

    lineCornerCurvedRectangle(painter, pen, start, end, roundRadius);

    const QPoint stringPos(start.x() + width  / 2 - strSize.width()  / 2 + strCompen.x(),
		                       start.y() + height / 2 - strSize.height() / 2 + strCompen.y());

    painter.setFont(fnt);
    painter.setBrush(strBr);
    painter.drawText(stringPos, str);

    return stringPos;
	}
}