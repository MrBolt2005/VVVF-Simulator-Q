#include "GenerateFFT.hpp"

// Standard Library
#include <optional>
#include <type_traits>
// Internal
#include "../../GenerateBasic.hpp"
#include "../../QtVideoWriter.hpp"
#include "../../../Yaml/MasconControl/YamlMasconAnalyze.hpp"
// Packages
#include <kissfft/kissfft.hh>
#include <QApplication>
#include <QFuture>
#include <QPainter>
#include <QPointF>
#include <QtConcurrent/QtConcurrent>

namespace VvvfSimulator::Generation::Video::FFT
{
	using namespace NAMESPACE_VVVF::InternalMath;

	QVector<std::complex<qreal>> GenerateFFT::FFTNAudio(const QVector<WaveValues> &waveForm)
	{
		static_assert(std::is_same<kissfft<qreal>::cpx_t, std::complex<qreal>>::value);
		const auto &N = waveForm.size();

		// Prepare input data for kissfft
		const QVector<qreal> window = hammingWindow(N);
		QVector<kissfft<qreal>::cpx_t> input(N); // let cpx_t = std::complex
		for (qsizetype i = 0; i < N; i++) input[i] = std::complex<qreal>((waveForm[i].U - waveForm[i].V) * window[i], 0.0);

		// Perform FFT using kissfft
		kissfft<qreal> fft_obj(N, false);
		QVector<std::complex<qreal>> fft(N);
		fft_obj.transform(input.data(), fft.data());

		return fft;
	}
	QImage GenerateFFT::getImage(VvvfValues control, const YamlVvvfSoundData &sound, const QSize &size, bool darkMode)
	{
			control.allowRandomFreqMove = false;
			QVector<WaveValues> PWM_Array = VvvfSimulator::Generation::GenerateBasic::WaveForm::getUVWSec(control, sound, m_PI_6, std::pow(2, Pow) - 1, false);
			QVector<std::complex<qreal>> FFT = FFTNAudio(PWM_Array);

			QImage image(size, QImage::Format_RGB888);
			QPainter painter(&image);

			image.fill(darkMode ? QColorConstants::Black : QColorConstants::White);

			static const QPen blackPen = QPen(QColorConstants::Black), whitePen = QPen(QColorConstants::White);
			painter.setPen(darkMode ? whitePen : blackPen);

			const int64_t width = size.width(), height = size.height();

			for (int i = 0; i < width - 1; i++)
			{
				const auto Rindex = static_cast<qsizetype>(m_PI * i);
				const qreal Ri = std::abs(FFT[Rindex]);
				const qreal Rii = std::abs(FFT[Rindex + 1]);
				const QPointF start(i, height - Ri * static_cast<qreal>(height * 2));
				const QPointF end(i + 1, height - Rii * static_cast<qreal>(height * 2));
				painter.drawLine(start, end);
			}

			return image;
	}
	void GenerateFFT::exportVideo(
		const GenerationBasicParameter &parameter,
		const QDir &fileName,
		const QSize &size,
		double fps,
		AVCodecID codecID,
		bool darkMode,
		bool startWait,
		bool endWait)
	{
		using GenerationVideoWriter = VvvfSimulator::Generation::GenerationCommon::GenerationVideoWriter;
		using ProgressData = VvvfSimulator::Generation::GenerationCommon::GenerationBasicParameter::ProgressData;
		using YamlMasconDataCompiled = NAMESPACE_YAMLMASCONCONTROL::YamlMasconAnalyze::YamlMasconDataCompiled;
		using YamlVvvfSoundData = NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;
		
		std::optional<BitmapViewer> viewer(std::nullopt);
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

		const YamlVvvfSoundData &vvvfData = parameter.soundData;
		const YamlMasconDataCompiled &masconData = parameter.masconData;
		ProgressData progressData = parameter.progress;

		VvvfValues control;
		
		control.allowRandomFreqMove = false;

		GenerationVideoWriter vr(fileName, size.width(), size.height(), fps, codecID, av::PixelFormat(AV_PIX_FMT_RGB24), true);

		// No longer really necessary because the opening will throw if it fails
		//if (!vr.isOpen()) return;

		// Progress Initialize
		progressData.total = masconData.getEstimatedSteps((1.0 / fps) + 120);

		const bool &START_WAIT = startWait;
		if (START_WAIT) vr.addEmptyFrames(fps, darkMode);

		// PROGRESS CHANGE
		progressData.progress += fps;

		bool loop = true;
		QImage currentImage;
		const auto getNextImage = [&]() { return getImage(control, vvvfData, size, darkMode); };
    QFuture<QImage> futureImage = QtConcurrent::run(getNextImage);

    while (loop)
    {
			control.sinTime = 0.0;
			control.sawTime = 0.0;

			currentImage = futureImage.result();
			vr.writeFrame(currentImage);

			viewer->setPixmap(QPixmap::fromImage(currentImage));

			futureImage = QtConcurrent::run(getNextImage);

			//viewer->setImage(currentImage);

			if (progressData.cancel) loop = false;
			else loop = masconData.checkForFreqChange(control, vvvfData, 1.0 / fps);

			// PROGRESS CHANGE
			progressData.progress++;
    }

		const bool &END_WAIT = endWait;
		if (END_WAIT) vr.addEmptyFrames(fps, darkMode);

		vr.close();

		viewer->close();
	}
	void GenerateFFT::exportImage(const QDir &fileName, const YamlVvvfSoundData &soundData, double d, const QSize &size, bool darkMode, const char* exportFormat, QImage* output)
	{
		using GenerationVideoWriter = VvvfSimulator::Generation::GenerationCommon::GenerationVideoWriter;
		using ProgressData = VvvfSimulator::Generation::GenerationCommon::GenerationBasicParameter::ProgressData;
		using YamlMasconDataCompiled = NAMESPACE_YAMLMASCONCONTROL::YamlMasconAnalyze::YamlMasconDataCompiled;
		using YamlVvvfSoundData = NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;
		
		std::optional<BitmapViewer> viewer(std::nullopt);
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

		VvvfValues control;
		
		control.allowRandomFreqMove = false;

		control.sinAngleFreq = d * m_2PI;
		control.controlFrequency = d;

		const QImage image = getImage(control, soundData, size, darkMode);
		viewer->setPixmap(QPixmap::fromImage(image));
		image.save(fileName.absolutePath(), exportFormat);
		if (output) *output = image;
	}
}