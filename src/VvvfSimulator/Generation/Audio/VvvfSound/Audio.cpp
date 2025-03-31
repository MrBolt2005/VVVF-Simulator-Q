#include "Audio.hpp"
// Standard Library Includes
#include <cmath>
#include <cstring>
#include <optional>
// Package Includes
#include <avcpp/audioresampler.h>
#include <avcpp/frame.h>
#include <QAudioDecoder>
#include <QDateTime>
#include <QUrl>

namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio
{
	BufferedWaveFileWriter::BufferedWaveFileWriter(const QDir& path, int samplingFrequency, QObject *parent = nullptr) :
		QObject(parent),
		m_Buffer(new BufferedWaveIODevice(this, 80000)),
		//m_audioSink(nullptr),
		m_file(path.path())
	{
		QAudioFormat format;
		format.setSampleRate(samplingFrequency);
		format.setChannelCount(1);
		format.setSampleFormat(QAudioFormat::Float);

		m_audioSink = new QAudioSink(format, this);

		if (!m_file.open(QIODevice::WriteOnly))
			qWarning() << QObject::tr("Não foi possível abrir o arquivo de saída");

		// Start the sink as soon as the object gets constructed.
		m_audioSink->start(m_Buffer);
	}

	BufferedWaveFileWriter::~BufferedWaveFileWriter()
	{
		if (m_audioSink) m_audioSink->stop();
		if (m_Buffer->size() != 0)
		{
			const auto size = m_Buffer->size();
			const auto data = m_Buffer->read(size);
			m_file.write(data.constData(), size);
		}
		m_file.close();
	}

	void BufferedWaveFileWriter::addSample(const QByteArray &sample)
	{
		m_Buffer->addSample(sample);
		if (m_Buffer->size() >= m_Buffer->maxSize())
		{
			const auto size = m_Buffer->size();
			const auto data = m_Buffer->read(size);
			m_file.write(data.constData(), size);
		}
	}

	/*
	void BufferedWaveFileWriter::start()
	{
		m_audioSink->start(m_Buffer.get());
	}
	*/

	QByteArray BufferedWaveFileWriter::floatToByteArray(float value)
	{
		QByteArray out;
		out.append(reinterpret_cast<const char*>(&value), sizeof(float));
		return out;
	}

	std::expected<void, QString> reSample(int newSamplingRate, const QDir &inputPath, const QDir &outputPath, bool deleteOld)
	{
		QAudioDecoder decoder;
		decoder.setSource(QUrl::fromLocalFile(inputPath.path()));

		const av::SampleFormat format(getAVSampleFormat(decoder.audioFormat().sampleFormat()));
		if (format == AV_SAMPLE_FMT_NONE)
			return std::unexpected(QObject::tr("Formato de amostra desconhecido"));

		QFile outFile(outputPath.path());
		if (!outFile.open(QIODevice::WriteOnly))
			return std::unexpected(QObject::tr("Não foi possível abrir o arquivo de saída"));
		
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

			bool finished = false;
			QObject::connect(&decoder, &QAudioDecoder::finished, [&]() {
				outFile.close();
				if (deleteOld) QFile::remove(inputPath.path());
				finished = true;
			});

			decoder.start();
			while (!finished);
			return error ? std::unexpected(*error) : std::expected<void, QString>();
		}
		catch (const av::Exception &e)
		{
			return std::unexpected(
				QObject::tr("Audio resampling error, category (%1), code %2: %3").arg(e.code().category().name()).arg(e.code().value()).arg(e.what())
			);
		}
	}
	void exportWavFile(GenerationCommon::GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, const QDir& Path)
	{
		constexpr float volumeFactor = 0.35f;
		const double dt = 1.0 / samplingFreq;
		genParam.progress.total = genParam.masconData.getEstimatedSteps(dt);
		if (!useRaw) genParam.progress.total *= std::pow(1.04, Path.entryList().size());

		NAMESPACE_VVVF::Struct::VvvfValues control{};
		constexpr int downSampledFrequency = 44100;

		const QDir exportPath = useRaw ? Path : Path.absolutePath() + '/' + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".temp";
		BufferedWaveFileWriter writer(exportPath, samplingFreq);

		bool loop = true;
		while (loop)
		{
			control.sinTime += dt;
			control.sawTime += dt;
			std::vector<float> samples = getSample(control, genParam.soundData);

			for (const float &sample : samples) writer.addSample(writer.floatToByteArray(sample * volumeFactor));
			genParam.progress.progress++;
			bool flagContinue = genParam.masconData.checkForFreqChange(control, genParam.soundData, 1.0 / samplingFreq);
			loop = !genParam.progress.cancel && flagContinue;
		}

		writer.close();
		if (!useRaw)
		{
			reSample(downSampledFrequency, exportPath, Path, true);
			genParam.progress.progress *= 1.05;
		}

		genParam.progress.progress = genParam.progress.total;
	}

	void exportWavLine(GenerationCommon::GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const QDir &Path)
	{
		GetSampleFunctional sampleGen = [&](
			NAMESPACE_VVVF::Struct::VvvfValues VVVF, NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData soundData
		) -> std::vector<float>
		{
			NAMESPACE_VVVF::Struct::PwmCalculateValues calculatedValues = YamlVvvfWave::calculateYaml(control, soundData);
			NAMESPACE_VVVF::Struct::WaveValues value = Calculate::calculatePhases(control, calculatedValues, 0);
			double pwmValue = (2.0 * value.U - value.V - value.W) * (const double)(1.0 / 8.0);
			return { static_cast<float>(pwmValue) };
		}
		return exportWavLine(genParam, sampleGen, samplingFreq, useRaw, Path);
	}
} // namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio