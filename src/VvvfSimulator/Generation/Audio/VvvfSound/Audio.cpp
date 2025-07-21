#include "Audio.hpp"
// Standard Library
#include <cmath>
#include <cstring>
#include <optional>
#include <string>
// Packages
#include <avcpp/audioresampler.h>
#include <avcpp/frame.h>
#include <QAudioDecoder>
#include <QDateTime>
#include <QDebug>
#include <QObject>
#include <QUrl>
// Internal
#include "../../../Vvvf/Calculate.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfWave.hpp"

#define WARN_FILE_ERROR \
m_warning = { \
	"Could not open the file (%1) on object creation: %2", \
	{{m_file.fileName()}, {m_file.errorString()}} \
}; \
m_isWarnLast = true;

namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio
{
	BufferedWaveFileWriter::BufferedWaveFileWriter
	(
		const std::filesystem::path &path,
		int samplingFrequency,
		BufferedWaveIODevice::size_type maxBufferSize,
		bool openOnCreation,
		bool startOnCreation
	)
		: m_file(path)
	/*
		//, m_file()
		//, m_audioSink(nullptr)
		//, m_isRunning(false)
	*/
	{
		QAudioFormat format;
		format.setSampleRate(samplingFrequency);
		format.setChannelCount(1);
		format.setSampleFormat(QAudioFormat::Float);

		m_Buffer.reset(new BufferedWaveIODevice(
			format, true, maxBufferSize < 0 ? samplingFrequency : maxBufferSize
		));
		m_audioSink.emplace(format);

		if (openOnCreation && !path.empty() && !m_file.open(QIODevice::WriteOnly))
			WARN_FILE_ERROR

		// Start the sink as soon as the object gets constructed.
		if (startOnCreation)
			if (!start()) WARN_FILE_ERROR
	}

	BufferedWaveFileWriter::~BufferedWaveFileWriter()
	{
		close();
	}

	qint64 BufferedWaveFileWriter::addSample(const QByteArrayView &sample)
	{
		if (!m_isRunning) return -1;

		qint64 retVal = sample.size();
		m_Buffer->write(sample.constData(), retVal);

		if (const qint64 size = m_Buffer->size(); size >= m_Buffer->maxSize())
		{
			const QByteArray data = m_Buffer->read(size);
			if (const qint64 ret2 = m_file.write(data); ret2 > 0) retVal += ret2;
		}

		return retVal;
	}

	void BufferedWaveFileWriter::close()
	{
		// Stop if running
		if (m_isRunning)
		{
			if (m_audioSink) m_audioSink->stop();
			if (m_Buffer->size() != 0)
			{
				const auto size = m_Buffer->size();
				const QByteArray data = m_Buffer->read(size);
				m_file.write(data);
			}
			m_isRunning = false;
		}

		return m_file.close();
	}

	QFile::FileError BufferedWaveFileWriter::fileError() const { return m_file.error(); }

	QString BufferedWaveFileWriter::fileErrorString() const { return m_file.errorString(); }

	bool BufferedWaveFileWriter::isOpen() const { return m_file.isOpen(); }

	std::filesystem::path BufferedWaveFileWriter::fileName() const
	{
		return m_file.filesystemFileName();
	}

	bool BufferedWaveFileWriter::open(const std::filesystem::path *const path = nullptr)
	{
		if (m_file.fileName().isEmpty() && !path) return false;
		if (m_file.isOpen())
		{
			// qDebug() << QObject::tr("NOTICE: Attempting to open already opened object file.");
			return true;
		}
		if (path) m_file.setFileName(*path);
		const bool retVal = m_file.open(QIODevice::WriteOnly);
		// if (!retVal) WARN_FILE_ERROR;
		return retVal;
	}

	bool BufferedWaveFileWriter::setFileName(const std::filesystem::path &path)
	{ 
		if (m_isRunning || m_file.isOpen()) return false;
		m_file.setFileName(path);
		return true;
	}

	bool BufferedWaveFileWriter::start()
	{
		if (m_file.filesystemFileName().empty() || !open())
			return false;
		if (!m_isRunning)
		{
			m_audioSink->start(m_Buffer.get());
			m_isRunning = true;
		}
		return true;
	}

	bool BufferedWaveFileWriter::stop()
	{
		if (!m_file.isOpen()) return false;
		
		if (m_isRunning)
		{
			if (m_audioSink) m_audioSink->stop();
			if (m_Buffer->size() != 0)
			{
				const auto size = m_Buffer->size();
				const QByteArray data = m_Buffer->read(size);
				m_file.write(data);
			}
			m_isRunning = false;
		}

		return true;
	}

	// End of BufferedWaveFileWriter

	Outcome::QStringResult<bool> reSample(int newSamplingRate, const QString &inputPath, const std::filesystem::path &outputPath, bool deleteOld)
	{
		static_assert(sizeof(QChar) == sizeof(char16_t));
		
		QAudioDecoder decoder;
		decoder.setSource(inputPath);

		const av::SampleFormat format(getAVSampleFormat(decoder.audioFormat().sampleFormat()));
		if (format == AV_SAMPLE_FMT_NONE)
			return QObject::tr("Formato de amostra desconhecido");

		QFile outFile(outputPath);
		if (!outFile.open(QIODevice::WriteOnly))
			return QObject::tr("Não foi possível abrir o arquivo de saída");
		
		const auto inputSampleRate = decoder.audioFormat().sampleRate();
		const auto inputChannels = decoder.audioFormat().channelCount();
		try
		{
			AVChannelLayout channelLayout;
			av_channel_layout_default(&channelLayout, inputChannels);
			av::AudioResampler resampler(
				channelLayout.u.mask,
				newSamplingRate,
				format,
				channelLayout.u.mask,
				inputSampleRate,
				format
			);

			QObject::connect(&decoder, &QAudioDecoder::bufferReady, [&]()
			{
				const QAudioBuffer buffer = decoder.read();
				const auto inputSamples = buffer.sampleCount();
				const auto inputDataBytes = buffer.constData<char>();
				
				// Push input data to the resampler
				av::AudioSamples inputSamplesWrapper(format, inputSamples, channelLayout.u.mask, inputSampleRate);
				std::memcpy(inputSamplesWrapper.data(), inputDataBytes, buffer.byteCount());
				resampler.push(inputSamplesWrapper);

				// Pop resampled data from the resampler
				av::AudioSamples outputSamples;
				while (resampler.pop(outputSamples, true)) // Get all available samples
				{
					outFile.write(reinterpret_cast<const char *>(outputSamples.data()), outputSamples.size());
				}
			});

			std::optional<QString> error;
			QObject::connect(&decoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), [&](QAudioDecoder::Error)
			{
				error = QObject::tr("Audio decoding error: %1").arg(decoder.errorString());
			});

			volatile bool finished = false;
			bool retVal = true;
			QObject::connect(&decoder, &QAudioDecoder::finished, [&]() {
				outFile.close();
				if (deleteOld)
					if (!QFile::remove(inputPath)) retVal = false;
				finished = true;
			});

			decoder.start();
			while (!finished);
			if (error) return *error;
			// else
			return retVal;
		}
		catch (const av::Exception &e)
		{
			return QObject::tr("Audio resampling error, category (%1), code %2: %3").arg(e.code().category().name()).arg(e.code().value()).arg(e.what());
		}
	}
	void exportWavFile(GenerationCommon::GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, const std::filesystem::path& Path)
	{
		constexpr float volumeFactor = 0.35f;
		const double dt = 1.0 / samplingFreq;
		genParam.progress.total = genParam.masconData.getEstimatedSteps(dt);
		if (!useRaw)
		{
			size_t pathEntryListSize = 0;
			for (auto it = Path.begin(); it != Path.end(); it++) pathEntryListSize++;
			genParam.progress.total *= std::pow(1.04, pathEntryListSize);
		}

		NAMESPACE_VVVF::Struct::VvvfValues control{};
		constexpr int downSampledFrequency = 44100;

		const std::filesystem::path exportPath = useRaw ? Path : (Path
			/ QDateTime::currentDateTime().toString(QStringLiteral(
				"yyyyMMddhhmmss")
			).toStdU16String())
			/ ".temp";
		BufferedWaveFileWriter writer(exportPath, samplingFreq, -1);

		bool loop = true;
		while (loop)
		{
			control.sinTime += dt;
			control.sawTime += dt;
			std::vector<float> samples = getSample(control, genParam.soundData);

			for (const float &sample : samples) writer.addSample({sample * volumeFactor});
			genParam.progress.progress++;
			bool flagContinue = genParam.masconData.checkForFreqChange(control, genParam.soundData, 1.0 / samplingFreq);
			loop = !genParam.progress.cancel && flagContinue;
		}

		writer.close();
		if (!useRaw)
		{
			const auto in = exportPath.u16string();
			const QString input = QString::fromRawData(reinterpret_cast<const QChar *>(in.data()), in.size());
			reSample(downSampledFrequency, input, Path, true);
			genParam.progress.progress *= 1.05;
		}

		genParam.progress.progress = genParam.progress.total;
	}

	void exportWavLine(GenerationCommon::GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const std::filesystem::path &Path)
	{
		static const GetSampleFunctional sampleGen = [](
			NAMESPACE_VVVF::Struct::VvvfValues &control,
			const NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData &soundData
		) -> std::vector<float>
		{
			NAMESPACE_VVVF::Struct::PwmCalculateValues calculatedValues = Yaml::VvvfSound::YamlVvvfWave::calculateYaml(control, soundData);
			NAMESPACE_VVVF::Struct::WaveValues value = Vvvf::Calculate::calculatePhases(control, calculatedValues, 0);
			double pwmValue = (2.0 * value.U - value.V - value.W) * (const double)(1.0 / 8.0);
			return { static_cast<float>(pwmValue) };
		};
		return exportWavFile(genParam, sampleGen, samplingFreq, useRaw, Path);
	}
} // namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio

#undef WARN_FILE_ERROR