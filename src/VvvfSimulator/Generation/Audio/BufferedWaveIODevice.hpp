#pragma once

// Package Includes
#include <QAudioFormat>
// Internal
#include "../Util/CircularBuffer.hpp"

namespace VvvfSimulator::Generation::Audio
{
	// Allow injecting individual samples at a time for a Qt audio sink buffer.
	class BufferedWaveIODevice : public Util::CircularBuffer
	{
		Q_OBJECT
		Q_PROPERTY(bool readFully READ readFully WRITE setReadFully)

		QAudioFormat m_fmt;
		bool m_readFully;

	public:
		explicit BufferedWaveIODevice(
			const QAudioFormat &fmt = QAudioFormat(),
			bool readFully = true,
			size_type maxSize = 16384,
			QObject *parent = nullptr
		);
		~BufferedWaveIODevice() override;

		virtual qint64 bufferDuration(const QAudioFormat *const maybeAnotherFmt = nullptr) const;
		constexpr bool readFully() const noexcept { return m_readFully; }
		constexpr QAudioFormat waveFormat() const noexcept { return m_fmt; }
		constexpr const QAudioFormat &waveFormatRef() const noexcept { return m_fmt; }

	/*
	signals:
    void readFullyChanged(bool arg);
    void waveFormatChanged(const QAudioFormat &fmt);
	*/

	public slots:
		virtual bool setReadFully(const bool rf);
		virtual bool setWaveFormat(const QAudioFormat &fmt);

	protected:
		qint64 readData(char *data, qint64 maxLen) override;
	};
}