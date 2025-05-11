// Internal Includes
#include "../BufferedWaveIODevice.hpp"
#include "../../GenerateCommon.hpp"
#include "../../../Outcome.hpp"
#include "../../../Vvvf/Struct.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"
// STL Includes
#include <filesystem>
#include <functional>
#include <optional>
#include <vector>
// Package Includes
#include <QAudioFormat>
#include <QAudioSink>
#include <QByteArray>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QScopedPointer>
#include <QString>

namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio
{
	class BufferedWaveFileWriter
	{
		Q_GADGET

		QFile m_file;
		QScopedPointer<BufferedWaveIODevice> m_Buffer;
		std::optional<QAudioSink> m_audioSink;
		bool m_isRunning = false;

	public:
		BufferedWaveFileWriter
		(
			const std::filesystem::path *const path = nullptr,
			int samplingFrequency = 80000,
			qint64 maxBufferSize = 80000,
			bool openOnCreation = true,
			bool startOnCreation = true
		);
		//BufferedWaveFileWriter(const QDir& path, const QAudioFormat& audioFormat, QObject *parent = nullptr);
		~BufferedWaveFileWriter();

		std::filesystem::path fileName() const;
		bool setFileName(const std::filesystem::path &path);
		
		qint64 addSample(const QByteArray& sample);												// OK
		void close();																											// OK
		QFile::FileError fileError() const;																// OK
		QString fileErrorString() const;																	// OK
		bool isOpen() const;																							// OK
		constexpr bool isRunning() const noexcept { return m_isRunning; } // OK
		bool open(const std::filesystem::path *const path = nullptr);			// OK
		bool start();																											// OK
		bool stop();																											// OK

		template <typename T>
		static QByteArray rawToByteArray(const T &value)
		{
			return QByteArray(reinterpret_cast<const char*>(&value), size);
		}
	};
	static constexpr AVSampleFormat getAVSampleFormat(QAudioFormat::SampleFormat sampleFormat)
	{
		// Fun fact: The Qt sample format enum values are the same as the FFmpeg ones + 1
		switch (sampleFormat)
		{
		case QAudioFormat::UInt8: return AV_SAMPLE_FMT_U8;
		case QAudioFormat::Int16: return AV_SAMPLE_FMT_S16;
		case QAudioFormat::Int32: return AV_SAMPLE_FMT_S32;
		case QAudioFormat::Float: return AV_SAMPLE_FMT_FLT;
		default:                  return AV_SAMPLE_FMT_NONE; // QAudioFormat::Unknown
		}
	}
	
	static Outcome::QStringResult<bool> reSample(int newSamplingRate, const QDir& inputPath, const std::filesystem::path& outputPath, bool deleteOld);

	using GetSampleFunctional = std::function<std::vector<float>(
		NAMESPACE_VVVF::Struct::VvvfValues &,
		const NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData &
	)>;

	static void exportWavFile(GenerationCommon::GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, const std::filesystem::path& Path);

//	public:
	static void exportWavLine(GenerationCommon::GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const std::filesystem::path& Path);
}
