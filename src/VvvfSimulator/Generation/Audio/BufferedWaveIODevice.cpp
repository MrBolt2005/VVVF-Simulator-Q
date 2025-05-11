#include "BufferedWaveIODevice.hpp"
// Standard Library Includes
#include <cstring>
// Package Includes
#include <QDebug>

namespace VvvfSimulator::Generation::Audio
{
	BufferedWaveIODevice::BufferedWaveIODevice(qint64 maxSize, bool __clearDataAfterRead, QObject *parent)
		: QIODevice(parent)
		, m_maxSize(maxSize)
		, clearDataAfterRead(__clearDataAfterRead)
	{}

	BufferedWaveIODevice::~BufferedWaveIODevice() = default;

	void BufferedWaveIODevice::addSample(const QByteArray &sample)
	{
		if (m_buffer.size() + sample.size() > m_maxSize)
			m_buffer.remove(0, sample.size()); // Discard on buffer overflow
		m_buffer.append(sample);
	}

	bool BufferedWaveIODevice::open(OpenMode mode)
	{
		const bool ret = QIODevice::open(mode);
		if (ret) m_pos = 0;
		return ret;
	}

	qint64 BufferedWaveIODevice::readData(char *data, qint64 maxlen)
	{
		if (m_pos >= m_buffer.size()) return -1;
		qint64 len = qMin(maxlen, m_buffer.size() - m_pos);
		std::memcpy(data, m_buffer.constData() + m_pos, len);
		if (clearDataAfterRead)
		{
			m_buffer.remove(0, len);
			m_pos = 0;
		}
		else m_pos += len;
		return len;
	}

	qint64 BufferedWaveIODevice::writeData(const char *data, qint64 len)
	{
		qWarning() << QObject::tr("This function/method (writeData) cannot be used on this class."); // Log de aviso
	//	qWarning("%s: %s\n", __PRETTY_FUNCTION__, qUtf8Printable(QObject::tr("A função writeData não pode ser usada nesta classe.")));
		Q_UNUSED(data);
		Q_UNUSED(len);
		return -1;
	}

	bool BufferedWaveIODevice::setMaxSize(qint64 maxSize)
	{
		if (maxSize <= 0) return false;

		m_maxSize = maxSize;

		// Ensure buffer doesn't exceed the new maximum size
    if (m_buffer.size() > m_maxSize)
		{
			// Trim from beginning (oldest data)
			m_buffer.remove(0, m_buffer.size() - m_maxSize);
			// Adjust position if needed
			m_pos = qMin(m_pos, m_buffer.size());
		}

		return true;
	}

	qint64 BufferedWaveIODevice::bytesAvailable() const
	{
		return m_buffer.size() - m_pos;
	}

	qint64 BufferedWaveIODevice::size() const
	{
		return m_buffer.size();
	}
}
