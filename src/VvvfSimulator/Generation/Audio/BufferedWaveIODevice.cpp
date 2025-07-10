#include "BufferedWaveIODevice.hpp"
// Standard Library Includes
#include <vector>
// Package Includes
#include <QDebug>

namespace VvvfSimulator::Generation::Audio
{
	BufferedWaveIODevice::BufferedWaveIODevice(
		const QAudioFormat &fmt,
		bool readFully,
		size_type maxSize,
		QObject *parent
	)
		: CircularBuffer(maxSize, parent)
		, m_fmt(fmt)
		, m_readFully(readFully)
	{}

	BufferedWaveIODevice::~BufferedWaveIODevice() = default;
	
	qint64 BufferedWaveIODevice::bufferDuration(const QAudioFormat *const maybeAnotherFmt) const
	{
		static_assert(sizeof(int) == sizeof(qint32));
		
		const auto fmt = maybeAnotherFmt ? maybeAnotherFmt : &m_fmt;
		const qint64 bytes = bytesAvailable();
		qint64 retVal = fmt->durationForBytes(bytes & 0xFFFFFFFF);
		if (const qint64 masked = bytes & 0xFFFFFFFF00000000; masked)
			retVal += fmt->durationForBytes(masked >> 32) << 32;
		return retVal;
	}

	qint64 BufferedWaveIODevice::readData(char *data, qint64 maxlen)
	{
		const qint64 read = CircularBuffer::readData(data, maxlen);

		if (m_readFully && read < maxlen)
		{
			const std::vector<char> empty(maxlen - read);
			const qint64 result = write(empty.data(), empty.size() * sizeof(char));
			Q_ASSERT(result == empty.size());
			return result;
		}

		return read;
	}

	bool BufferedWaveIODevice::setReadFully(const bool rf)
	{
		if (isOpen()) return false;
				
		m_readFully = rf;
		/* emit readFullyChanged(m_readFully); */
		return true;
	}

	bool BufferedWaveIODevice::setWaveFormat(const QAudioFormat &fmt)
	{
		if (isOpen()) return false;
			
		m_fmt = fmt;
		/* emit waveFormatChanged(m_fmt); */
		return true;
	}
}
