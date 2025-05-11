#pragma once

// Package Includes
#include <QBuffer>
#include <QByteArray>
#include <QIODevice>

namespace VvvfSimulator::Generation::Audio
{
	// Allow injecting individual samples at a time for a Qt audio sink buffer.
	class BufferedWaveIODevice : public QIODevice
	{
		Q_OBJECT
		Q_PROPERTY(qint64 maxSize READ maxSize WRITE setMaxSize)

		QByteArray m_buffer;
		qint64 m_pos = 0;
		qint64 m_maxSize = 80000;
	public:
		bool clearDataAfterRead = true;
		
		BufferedWaveIODevice(qint64 maxSize = 80000, bool __clearDataAfterRead = true, QObject *parent = nullptr);
		~BufferedWaveIODevice() override;

		qint64 bytesAvailable() const override;
		qint64 size() const override;
		constexpr qint64 maxSize() const noexcept
		{
			return m_maxSize;
		}
		
	public slots:
		void addSample(const QByteArray& sample);
		bool open(OpenMode mode) override;
		qint64 readData(char *data, qint64 maxlen) override;
		qint64 writeData(const char *data, qint64 len) override;
		bool setMaxSize(qint64 maxSize);
	};
}