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
#include <QByteArrayView>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QIODevice>
#include <QObject>
#include <QScopedPointer>
#include <QString>
// Internal
#include "../../../Util/String.hpp"

namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio
{
	class BufferedWaveFileWriter : public QIODevice
	{
		Q_GADGET

		QFile m_file;
		QScopedPointer<BufferedWaveIODevice> m_Buffer;
		std::optional<QAudioSink> m_audioSink;
		VvvfSimulator::Util::String::TranslatableFmtString m_warning;
		bool m_isWarnLast = true, m_isRunning = false;

	public:
		/*
		@brief Make a new BufferedWaveFileWriter object.

		@param path The destination file path. Required to use openOnCreation and 
		startOnCreation.
		@param samplingFrequency In Hertz (Hz).
		@param maxBufferSize Maximum size of internal BufferedWaveIODevice buffer.
		@param openOnCreation If true AND path is non-empty, will try to open the 
		destination file, but won't start it immediately unless startOnCreation is 
		true too.
		@param startOnCreation Start the writer (namely, its internal 
		BufferedWaveIODevice) if true AND path is non-empty.
		*/
		BufferedWaveFileWriter
		(
			const std::filesystem::path &path = std::filesystem::path(),
			int samplingFrequency = 80000,
			BufferedWaveIODevice::size_type maxBufferSize = 80000,
			bool openOnCreation = true,
			bool startOnCreation = true
		);
		//BufferedWaveFileWriter(const QDir& path, const QAudioFormat& audioFormat, QObject *parent = nullptr);
		/*
		@brief Destroys the object, stopping the buffer if it is running and 
		closing the destination file if it is opened.
		*/
		~BufferedWaveFileWriter();

		/*
		@brief Read the active file name path.
		*/
		std::filesystem::path fileName() const;
		/*
		@brief Write the active file name path. Will only work if the writer is 
		stopped and the associated file closed.

		@returns True if, and only if, the file name was changed successfully.
		Will fail if the writer is still active, or the original file path, open.
		@param path The new file name path to be set.
		*/
		bool setFileName(const std::filesystem::path &path);							// OK
		
		/*
		@brief Write a sample sequence represented by a raw byte array. The buffer 
		must be running (and, by extension, the associated file must be open) for 
		this operation to work successfully.
		
		@returns How many bytes were written to the file, or a negative value (-1) 
		if it fails.
		@param sample Byte array representation of the sample sequence to be 
		written. Make sure that the format contained on this matches the float 
		type.
		*/
		qint64 addSample(const QVector<float>& sample) 										// OK
		{
			return addSample(QByteArrayView(
				reinterpret_cast<const char *>(sample.constData()),
				sample.size() * sizeof(float) / sizeof(char))
			);
		}
		qint64 addSample(const QByteArrayView &sample);
		void close();																											// OK
		QFile::FileError fileError() const;																// OK
		QString fileErrorString() const;																	// OK
		bool isOpen() const;																							// OK
		constexpr bool isRunning() const noexcept { return m_isRunning; } // OK
		/*
		@brief Opens a file to be written by this object.

		@returns True if, and only if, either opens successfully or was already 
		open. Will fail if the provided path is empty (invalid) or the internal 
		QFile::open() call returns false, in which case its cause can be checked
		by calling either fileError() or fileErrorString().
		@param path If null, will use the already stored file name path accessible
		via	fileName(). Otherwise, will set the object's FN path to the one
		referenced by the parameter before trying to open the file.
		*/
		bool open(const std::filesystem::path *const path = nullptr);			// OK
		/*
		@brief Attempts to start the object's buffer.

		@returns True if, and only if, starts successfully or was already started.
		Will fail if the path attribute is empty (invalid) or it fails to open the
		associated file in case it was previously closed.
		*/
		bool start();																											// OK
		/*
		@brief Stops the object's buffer.

		@returns True if stops successfully or was already stopped.
		Will fail if the path attribute is empty (invalid).
		*/
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
	
	static Outcome::QStringResult<bool> reSample(int newSamplingRate, const QString& inputPath, const std::filesystem::path& outputPath, bool deleteOld);

	using GetSampleFunctional = std::function<std::vector<float>(
		NAMESPACE_VVVF::Struct::VvvfValues &,
		const NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData &
	)>;

	static void exportWavFile(GenerationCommon::GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, const std::filesystem::path& Path);

//	public:
	static void exportWavLine(GenerationCommon::GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const std::filesystem::path& Path);
}
