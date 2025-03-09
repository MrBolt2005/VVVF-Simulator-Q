#include "Audio.hpp"
// Standard Library Includes
#include <cmath>
// Package Includes
#include <QAudioDecoder>
#include <QDateTime>

namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio
{
	BufferedWaveFileWriter::BufferedWaveFileWriter(const QDir& path, int samplingFrequency, QObject *parent = nullptr) :
		QObject(parent),
		m_Buffer(new BufferedWaveIODevice(this, 80000)),
		//m_audioSink(nullptr),
		m_file(path.absolutePath())
	{
		QAudioFormat format;
		format.setSampleRate(samplingFrequency);
		format.setChannelCount(1);
		format.setSampleFormat(QAudioFormat::Float);

		m_audioSink = new QAudioSink(format, this);

		if (!m_file->open(QIODevice::WriteOnly))
			qWarning(QObject::tr("Não foi possível abrir o arquivo de saída"));

		// Start the sink as soon as the object gets constructed.
		m_audioSink->start(m_Buffer);
	}

	BufferedWaveFileWriter::~BufferedWaveFileWriter()
	{
		if (m_audioSink) m_audioSink->stop();
		if (m_Buffer->size() != 0)
		m_file.write(m_Buffer->readData(m_Buffer->size()));
		m_file.close();
	}

	void BufferedWaveFileWriter::addSample(const QByteArray &sample)
	{
		m_Buffer->addSample(sample);
		if (Buffer->size() >= m_Buffer->maxSize())
			m_file.write(m_Buffer->readData(m_Buffer->size()));
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

	void downSample(int newSamplingRate, const QDir &inputPath, const QDir &outputPath, bool deleteOld)
	{
		QAudioDecoder decoder;
		decoder.setSourceFilename(inputPath.absolutePath());

		QAudioFormat format;
		format.setSampleRate(newSamplingRate);
		format.setChannelCount(1);
		format.setSampleFormat(QAudioFormat::Float);

		QAudioSink sink(format, nullptr);
		QFile outFile(outputPath.absolutePath());
		if (!outFile.open(QIODevice::WriteOnly))
		{
			qWarning(QObject::tr("Não foi possível abrir o arquivo de saída"));
			return;
		}

		QObject::connect(&decoder, &QAudioDecoder::bufferReady, [&]() {
			QAudioBuffer buffer = decoder.read();
			QByteArray data = buffer.data();
			// Perform downsampling here if necessary
			sink.write(data);
		});

		QObject::connect(&decoder, QOverload<QAudioDecoder::Error>::of(&QAudioDecoder::error), [&](QAudioDecoder::Error error)
		{
			qWarning() << QObject::tr("Erro ao decodificar o áudio: %1").arg(decoder.errorString());
		});

		QObject::connect(&decoder, &QAudioDecoder::finished, [&]()
		{
			sink.stop();
			outFile.write(sink.readAll());
			outFile.close();
			if (deleteOld)
				QFile::remove(inputPath.absolutePath());
		});

		decoder.start();
	}
	void exportWavFile(GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, const QDir& Path)
	{
		constexpr float volumeFactor = 0.35f;
		const double dt = 1.0 / samplingFreq;
		genParam.progress.total = genParam.masconData.getEstimatedSteps(dt);
		if (!useRaw) genParam.progress.total *= std::pow(1.04, path.size()));

		VvvfValues control{};
		constexpr int downSampledFrequency = 44100;

		const QDir exportPath = useRaw ? Path : Path.absolutePath() + '/' + QDateTime.currentDateTime().toString("yyyyMMddhhmmss") + ".temp";
		BufferedWaveFileWriter writer(exportPath, samplingFreq);

		bool loop = true;
		while (loop)
		{
			control.time += dt;
			control.sawTime += dt;
			std::vector<float> samples = getSample(control, genParam.VvvfData);

			for (const float &sample : samples) writer.addSample(writer.floatToByteArray(sample * volumeFactor));
			genParam.progress.current++;
			bool flagContinue = YamlMasconControl.checkForFreqChange(control, genParam.masconData, 1.0 / samplingFreq);
			loop = !genParam.progress.cancel && flagContinue;
		}

		writer.close();
		if (!useRaw)
		{
			downSample(downSampledFrequency, exportPath, Path, true);
			genParam.progress.progress *= 1.05;
		}

		genParam.progress.progress = genParam.progress.total;
	}

	void exportWavLine(GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const QDir &Path)
	{
		GetSampleFunctional sampleGen = [&](VvvfValues VVVF, YamlVvvfSoundData soundData) -> std::vector<float>
		{
			PwmCalculateValues calculatedValues = YamlVvvfWave::calculateYaml(control, soundData);
			WaveValues value = Calculate::calculatePhases(control, calculatedValues, 0);
			double pwmValue = (2.0 * value.U - value.V - value.W) * (const double)(1.0 / 8.0);
			return { static_cast<float>(pwmValue) };
		}
		return exportWavLine(genParam, sampleGen, samplingFreq, useRaw, Path);
	}
} // namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio