#include "BufferedWaveIODevice.hpp"
// Standard Library Includes
#include <cstring>
// Package Includes
#include <QDebug>

VvvfSimulator::Generation::Audio::BufferedWaveIODevice::BufferedWaveIODevice(QObject *parent, qint64 maxSize, bool __clearDataAfterRead) :
	QIODevice(parent), m_maxSize(maxSize), clearDataAfterRead(__clearDataAfterRead)
{}

VvvfSimulator::Generation::Audio::BufferedWaveIODevice::~BufferedWaveIODevice()
{}

void VvvfSimulator::Generation::Audio::BufferedWaveIODevice::addSample(const QByteArray &sample)
{
	if (m_buffer.size() + sample.size() > m_maxSize)
		m_buffer.remove(0, sample.size()); // Discard on buffer overflow
	m_buffer.append(sample);
}

bool VvvfSimulator::Generation::Audio::BufferedWaveIODevice::open(OpenMode mode)
{
	bool ret = QIODevice::open(mode);
	if (ret) m_pos = 0;
	return ret;
}

qint64 VvvfSimulator::Generation::Audio::BufferedWaveIODevice::readData(char *data, qint64 maxlen)
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

qint64 VvvfSimulator::Generation::Audio::BufferedWaveIODevice::writeData(const char *data, qint64 len)
{
	qWarning("%s: %s\n", __PRETTY_FUNCTION__, qUtf8Printable(QObject::tr("A função writeData não pode ser usada nesta classe."))); // Log de aviso
	Q_UNUSED(data);
	Q_UNUSED(len);
	return -1;
}

qint64 VvvfSimulator::Generation::Audio::BufferedWaveIODevice::bytesAvailable() const
{
	return m_buffer.size() - m_pos;
}

qint64 VvvfSimulator::Generation::Audio::BufferedWaveIODevice::size() const
{
	return m_buffer.size();
}

qint64 VvvfSimulator::Generation::Audio::BufferedWaveIODevice::maxSize() const
{
	return m_maxSize;
}
