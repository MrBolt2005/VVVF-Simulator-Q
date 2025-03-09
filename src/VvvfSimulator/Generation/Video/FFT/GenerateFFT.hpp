#pragma once

// Standard Library
#include <complex>
#include <memory>
#include <pair>
#include <type_traits>
// Internal
#include "../../../GUI/Util/BitmapViewer.qml.hpp"
#include "../../GenerateCommon.hpp"
#include "../../../Vvvf/InternalMath.hpp"
#include "../../../Vvvf/Struct.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"
// Packages
#include <avcpp/codec.h>
#include <QDir>
#include <QGraphicsView>
#include <QImage>
#include <QSize>
#include <QVector>

namespace VvvfSimulator::Generation::Video::FFT
{
	using VvvfSimulator::Generation::GenerationCommon::GenerationBasicParameter;
	using VvvfSimulator::GUI::Util::BitmapViewer;
	using NAMESPACE_VVVF::Struct::VvvfValues;
	using NAMESPACE_VVVF::Struct::WaveValues;
	using NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;

	class GenerateFFT
	{
		static constexpr int Pow = 15;
		static inline QVector<qreal> hammingWindow(qsizetype N)
		{
			QVector<qreal> window(N);
			if constexpr (std::is_same<qreal, double>())
				for (int n = 0; n < N; ++n) window[n] = 0.54 - 0.46 * NAMESPACE_VVVF::InternalMath::Functions::cosine(2.0 * M_PI * n / (N - 1));
			else
				for (int n = 0; n < N; ++n) window[n] = 0.54 - 0.46 * std::cos(2.0 * M_PI * n / (N - 1));
			return window;
		}		
		static QVector<std::complex<qreal>> FFTNAudio(const QVector<WaveValues>& waveForm);

		//std::shared_ptr<BitmapViewer> m_viewer = nullptr;

	public:
		constexpr       std::shared_ptr<BitmapViewer>       viewer() const noexcept { return m_viewer; }
		constexpr const std::shared_ptr<BitmapViewer>& constViewer() const noexcept { return m_viewer; }

		/// <summary>
		/// Gets image of FFT.
		/// </summary>
		/// <param name="control">Make sure cloned data is passed</param>
		/// <param name="sound"></param>
		/// <returns></returns>
		static QImage getImage(VvvfValues control, const YamlVvvfSoundData &sound, const QSize &size = QSize(1000, 1000), bool darkMode = false);

		void exportVideo(
			const GenerationBasicParameter &parameter,
			const QDir &fileName,
			const QSize &size = QSize(1000, 1000),
			double fps = 60.0,
			AVCodecID codecID = AV_CODEC_ID_H264,
			bool darkMode = false,
			bool startWait = true,
			bool endWait = true);
		void exportImage(
			const QDir &fileName,
			const YamlVvvfSoundData &soundData,
			double d,
			const QSize &size = QSize(1000, 1000),
			//const QImage::Format &format = Format_RGB888,
			bool darkMode = false,
			const char* exportFormat = nullptr,
			QImage* output = nullptr);
	};
}