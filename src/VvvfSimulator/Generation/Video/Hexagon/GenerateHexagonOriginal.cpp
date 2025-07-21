#include "GenerateHexagonOriginal.hpp"

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

// GenerateHexagonOriginal.cpp
// Version 1.9.1.1

// Standard Library
#include <cfloat>
// Packages
#include <QPainter>
// Internal
#include "../../GenerateBasic.hpp"

namespace VvvfSimulator::Generation::Video::ControlInfo::GenerateHexagonOriginal
{
	QImage getImage(const QVector<WaveValues> &UVW, double controlFrequency, const QSize &size, qreal thickness, bool zeroVectorCircle, bool darkMode)
	{
		QImage imResult(size, QImage::Format_RGB32);
		imResult.fill(darkMode ? QColorConstants::Black : QColorConstants::White);
		if (controlFrequency == 0.0) return imResult;

		bool drawnCircle = false;
		QPointF currentPoint(0.0, 0.0),
		        maxValue(DBL_MIN, DBL_MIN),
						minValue(DBL_MAX, DBL_MAX);
		QVector<QPointF> zeroPoints, linePoints{ currentPoint };

		WaveValues preWaveValues(0, 0, 0);

		for (const auto &value : UVW)
		{
			QPointF deltaMove(
				-0.5 * value.W - 0.5 * value.V + value.U,
				-0.866025403784438646763 * value.W + 0.866025403784438646763 * value.V
			);

			if (preWaveValues != value)
			{
				linePoints.emplace_back(currentPoint);
				preWaveValues = value;
			}

			if (deltaMove.isNull() && zeroVectorCircle && !drawnCircle)
			{
				drawnCircle = true;
				zeroPoints.emplace_back(currentPoint);
			}
			else if (!deltaMove.isNull()) drawnCircle = false;
			currentPoint += deltaMove;

			maxValue = QPointF(
				qMax(maxValue.x(), currentPoint.x()),
				qMax(maxValue.y(), currentPoint.y())
			);
			minValue = QPointF(
				qMin(minValue.x(), currentPoint.x()),
				qMin(minValue.y(), currentPoint.y())
			);
		}

		QPainter pResult(&imResult);
		const double k = 1200.00 / (UVW.size() - 1);

		const QPointF correctionAmount(
			0.5 * size.width() - k * (minValue.x() + (maxValue.x() - minValue.x()) * 0.5),
			0.5 * size.height() - k * (minValue.y() + (maxValue.y() - minValue.y()) * 0.5)
		);

		pResult.setPen(QPen(darkMode ? QColorConstants::White : QColorConstants::Black, thickness));
		for (auto point = linePoints.begin(); point + 1 != linePoints.end(); point++)
		{
			const QPointF start(k * (*point) + correctionAmount),
			              end(k * (*(point + 1)) + correctionAmount);
			pResult.drawLine(start, end);
		}

		for (const auto &_point : zeroPoints)
		{
			const QPointF point(k * _point + correctionAmount);

			const double radius = 15.0 * ((controlFrequency > 40) ? 1 : (controlFrequency / 40.0));
			// Fill the ellipse
			pResult.setBrush(QBrush(darkMode ? QColorConstants::Black : QColorConstants::White));
			pResult.drawEllipse(QRectF(point.x() - radius, point.y() - radius, radius * 2, radius * 2));

			// Draw the ellipse border
			pResult.setPen(darkMode ? QColorConstants::White : QColorConstants::Black); //pResult.setPen(QPen(darkMode ? QColorConstants::White : QColorConstants::Black));
			pResult.drawEllipse(QRectF(point.x() - radius, point.y() - radius, radius * 2, radius * 2));
		}
		
		return imResult;
	}
}