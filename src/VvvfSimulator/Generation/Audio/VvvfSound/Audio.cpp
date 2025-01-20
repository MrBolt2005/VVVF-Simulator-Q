#include "Audio.hpp"
// Package Includes
#include <QDateTime>

namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio
{
	BufferedWaveFileWriter::BufferedWaveFileWriter(const QDir& path, int samplingFrequency, QObject *parent = nullptr) :
		QObject(parent), m_Buffer(nullptr), m_audioSink(nullptr), m_file(path)
	{
		QAudioFormat format;
		format.setSampleRate(samplingFrequency);
		format.setChannelCount(1);
		format.setSampleFormat(QAudioFormat::Float);

		m_Buffer = std::make_unique<BufferedWaveIODevice>(this, 80000);
		m_audioSink = std::make_unique<QAudioSink>(format, this);

		if (!m_file->open(QIODevice::WriteOnly))
			qWarning(QOBject::tr("Não foi possível abrir o arquivo de saída"));

		// Start the sink as soon as the object gets constructed.
		m_audioSink->start(m_Buffer.get());
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
}