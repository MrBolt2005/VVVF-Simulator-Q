#include "GenerateFourierSeries.hpp"

// Internal
#include "../../GenerateBasic.hpp"
// Packages
#include <QBrush>
#include <QPainter>
#include <QPen>

namespace VvvfSimulator::Generation::Video::FS::GenerateFourierSeries
{
  using namespace VvvfSimulator::Generation;
    
  QImage getImage(const QVector<double> &coefficients, int width, int height, bool darkMode)
  {
		QImage image(width, height, QImage::Format_RGB32);
		QPainter painter(&image);
		image.fill(darkMode ? QColorConstants::Black : QColorConstants::White);

		const qsizetype& count = coefficients.size();
		if (count == 0) return image;
		const qsizetype  barWidth = width / count;

		for (qsizetype i = 0; i < count; i++)
		{
			const double& result   = coefficients[i];
			const double  ratio    = result / GenerateBasic::Fourier::VoltageConvertFactor;
			const qsizetype barHeight = static_cast<qsizetype>(ratio * (height / 2));
			const QBrush brush = MagnitudeColor::getColor(ratio);
			if (barHeight < 0)	painter.fillRect(i * barWidth, height / 2, barWidth, -barHeight, brush);
			else                painter.fillRect(i * barWidth, (height / 2) - barHeight, barWidth, barHeight, brush);

			if (barWidth > 10 && i != 0 && i != count - 1)
			{
				static const QPen grayPen = QPen(QColorConstants::Gray);
				if (painter.pen() != grayPen) painter.setPen(grayPen);
				painter.drawLine(i * barWidth, 0, i * barWidth, height);
			}
		}

		return image;
	}
}