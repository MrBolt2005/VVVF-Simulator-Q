#include "GenerateControlOriginal.hpp"

// Standard Library
#include <optional>
// Packages
#include <QApplication>
#include <QColor>
#include <QFontDatabase>
#include <QFontMetricsF>
#include <QFuture>
#include <QLinearGradient>
#include <QObject>
#include <QPainter>
#include <QPointF>
#include <QtConcurrent/QtConcurrent>
// Internal
#include "GenerateControlCommon.hpp"

namespace VvvfSimulator::Generation::Video::ControlInfo
{
	QImage GenerateControlOriginal::getImage(
		const VvvfValues &control,
		bool finalShow,
		int width,
		int height,
		const QFont &titleFnt,
		const QFont &valFnt,
		const QFont &valMiniFnt,
		bool darkMode)
	{
		using GenerateControlCommon::getAlignment;

		QImage image(width, height, QImage::Format_RGB32);
		QPainter painter(&image);

		QColor gradationColor;
		if (control.freeRun)
			gradationColor = QColor(0xE0, 0xFD, 0xE0);
		else gradationColor = control.brake ? QColor(0xFD, 0xE0, 0xE0) : QColor(0xE0, 0xE0, 0xFD);

		QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(image.width(), image.height()));
		gradient.setColorAt(0.0, darkMode ? QColorConstants::Black : QColorConstants::White);
		gradient.setColorAt(1.0, gradationColor);

		painter.setBrush(QBrush(gradient));
		painter.drawRect(0, 0, image.width(), image.height());

		// Scaling factors
		const qreal xScale = static_cast<qreal>(width) / 500.0;
		const qreal yScale = static_cast<qreal>(height) / 1080.0;

		// Draw the top rectangle
		painter.setBrush(QColor(200, 200, 255));
		painter.drawRect(QRectF(0, 0, image.width(), 68 * yScale));

		// Set text brush
		const QBrush textBrush(darkMode ? QColorConstants::White : QColorConstants::Black);
		painter.setBrush(textBrush);

		// Function to draw text with auto-scaling font
		const auto drawTextAutoScale = [&](const QRectF &rect, const QString &text, QFont font) {
			QFontMetricsF metrics(font);
			while (metrics.horizontalAdvance(text) > rect.width() && font.pointSizeF() > 1) {
				font.setPointSizeF(font.pointSizeF() - 1);
				metrics = QFontMetricsF(font);
			}
			painter.setFont(font);
			painter.drawText(rect, getAlignment(font.family()) | Qt::AlignTop, text);
		};

		// Draw "Pulse Mode" text
		drawTextAutoScale(QRectF(17 * xScale, 8 * yScale, image.width() - 34 * xScale, titleFnt.pointSizeF()), QObject::tr("Pulse Mode"), titleFnt);

		// Draw the blue separator
		painter.setBrush(QColorConstants::Blue);
		painter.drawRect(QRectF(0, 68 * yScale, image.width(), 8 * yScale));

		if (!finalShow)
		{
			const QStringList pulseName = control.getPulseName();

			painter.setBrush(textBrush);
			drawTextAutoScale(QRectF(17 * xScale, 100 * yScale, image.width() - 34 * xScale, valFnt.pointSizeF()), pulseName[0], valFnt);

			if (pulseName.size() == 2)
			{
				drawTextAutoScale(QRectF(17 * xScale, 170 * yScale, image.width() - 34 * xScale, valMiniFnt.pointSizeF()), pulseName[1], valMiniFnt);
			}
			else if (pulseName.size() >= 3)
			{
				drawTextAutoScale(QRectF(17 * xScale, 160 * yScale, image.width() - 34 * xScale, valMiniFnt.pointSizeF()), pulseName[1], valMiniFnt);
				drawTextAutoScale(QRectF(17 * xScale, 180 * yScale, image.width() - 34 * xScale, valMiniFnt.pointSizeF()), pulseName[2], valMiniFnt);
			}
		}

		// Draw Sine Frequency text
		painter.setBrush(QColor(200, 200, 255));
		painter.drawRect(QRectF(0, 226 * yScale, image.width(), 65 * yScale));

		painter.setBrush(textBrush);
		drawTextAutoScale(QRectF(17 * xScale, 231 * yScale, image.width() - 34 * xScale, titleFnt.pointSizeF()), QObject::tr("Sine Freq[Hz]"), titleFnt);

		painter.setBrush(QColorConstants::Blue);
		painter.drawRect(QRectF(0, 291 * yScale, image.width(), 8 * yScale));

		const double &sineFreq = control.videoSineFrequency;
		if (!finalShow)
		{
			painter.setBrush(textBrush);
			drawTextAutoScale(QRectF(17 * xScale, 323 * yScale, image.width() - 34 * xScale, valFnt.pointSizeF()), QString::number(sineFreq, 'f', 2), valFnt);
		}

		// Draw Sine Amplitude text
		painter.setBrush(QColor(200, 200, 255));
		painter.drawRect(QRectF(0, 447 * yScale, image.width(), 66 * yScale));

		painter.setBrush(textBrush);
		drawTextAutoScale(QRectF(17 * xScale, 452 * yScale, image.width() - 34 * xScale, titleFnt.pointSizeF()), QObject::tr("Sine Amplitude[%]"), titleFnt);

		painter.setBrush(QColorConstants::Blue);
		painter.drawRect(QRectF(0, 513 * yScale, image.width(), 8 * yScale));

		if (!finalShow)
		{
			const double &sineAmp = control.videoSineAmplitude;
			painter.setBrush(textBrush);
			drawTextAutoScale(QRectF(17 * xScale, 548 * yScale, image.width() - 34 * xScale, valFnt.pointSizeF()), QString::number(sineAmp * 100, 'f', 2), valFnt);
		}

		// Draw Freerun text
		painter.setBrush(QColor(240, 240, 240));
		painter.drawRect(QRectF(0, 669 * yScale, image.width(), 66 * yScale));

		painter.setBrush(textBrush);
		drawTextAutoScale(QRectF(17 * xScale, 674 * yScale, image.width() - 34 * xScale, titleFnt.pointSizeF()), QObject::tr("Freerun"), titleFnt);

		painter.setBrush(QColorConstants::LightGray);
		painter.drawRect(QRectF(0, 735 * yScale, image.width(), 8 * yScale));

		if (!finalShow)
		{
			painter.setBrush(textBrush);
			drawTextAutoScale(QRectF(17 * xScale, 750 * yScale, image.width() - 34 * xScale, valFnt.pointSizeF()), control.masconOff ? QObject::tr("On") : QObject::tr("Off"), valFnt);
		}

		// Draw Braking text
		painter.setBrush(QColor(240, 240, 240));
		painter.drawRect(QRectF(0, 847 * yScale, image.width(), 66 * yScale));

		painter.setBrush(textBrush);
		drawTextAutoScale(QRectF(17 * xScale, 852 * yScale, image.width() - 34 * xScale, titleFnt.pointSizeF()), QObject::tr("Brake"), titleFnt);

		painter.setBrush(QColorConstants::LightGray);
		painter.drawRect(QRectF(0, 913 * yScale, image.width(), 8 * yScale));

		if (!finalShow)
		{
			painter.setBrush(textBrush);
			drawTextAutoScale(QRectF(17 * xScale, 930 * yScale, image.width() - 34 * xScale, valFnt.pointSizeF()), control.brake ? QObject::tr("On") : QObject::tr("Off"), valFnt);
		}

		return image;
	}

	void GenerateControlOriginal::exportVideo(
		GenerationCommon::GenerationBasicParameter &generationBasicParameter,
		const QDir &outputPath,
		double FPS,
		int width,
		int height,
		const QFont &titleFnt,
		const QFont &valFnt,
		const QFont &valMiniFnt,
		bool darkMode
	)
	{
		std::optional<VvvfSimulator::GUI::Util::BitmapViewer> viewer;
		QMetaObject::invokeMethod(
			QApplication::activeWindow(),
			[&]()
			{
				viewer.emplace();
			},
			Qt::QueuedConnection
		);
		while (!viewer.has_value());
		viewer->show();

		const auto &vvvfData = generationBasicParameter.soundData;
		const auto &masconData = generationBasicParameter.masconData;
		auto progressData = generationBasicParameter.progress;

		NAMESPACE_VVVF::Struct::VvvfValues control;

		GenerationCommon::GenerationVideoWriter vr(
			outputPath,
			width,
			height,
			FPS,
			AV_CODEC_ID_H264,
			av::PixelFormat(AV_PIX_FMT_RGB32),
			true
		);

		// Generate Opening
		{
		}

		bool loop = true, video_finished, final_show = false, first_show = true;
		double freeze_count = 0;

		progressData.total = masconData.getEstimatedSteps((1.0 / FPS) + FPS * 2.0);

		const auto getNextImage = [&]() { return getImage(control, final_show, width, height, titleFnt, valFnt, valMiniFnt, darkMode); };
		QFuture<QImage> futureImage = QtConcurrent::run(getNextImage);

		while (loop)
		{
			control.sinTime = 0.0;
			control.sawTime = 0.0;

			const QImage currentImage = futureImage.result();
			vr.writeFrame(currentImage);

			viewer->setPixmap(QPixmap::fromImage(currentImage));

			futureImage = QtConcurrent::run(getNextImage);

			// PROGRESS ADD
			progressData.progress++;

			if (first_show)
			{
				freeze_count++;
				if (freeze_count > FPS)
				{
					freeze_count = 0.0;
					first_show = false;
				}
				continue;
			}

			video_finished = !masconData.checkForFreqChange(control, vvvfData, 1.0 / FPS);
			if (video_finished)
			{
				final_show = true;
				freeze_count++;
			}
			if (freeze_count > FPS || progressData.cancel) loop = false;
		}
		vr.close();
		
		viewer->close();
	}
}