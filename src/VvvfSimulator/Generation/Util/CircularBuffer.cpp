#include "CircularBuffer.hpp"

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Generation/Util/CircularBuffer.cpp
 * v1.10.0.1
 */

// Packages
#include <QDebug>
#include <QLatin1StringView>
#include <QString>

namespace VvvfSimulator::Generation::Util {
CircularBuffer::CircularBuffer(size_type capacity, QObject *parent)
    : QIODevice(parent)
    , m_buffer(capacity) {}

CircularBuffer::~CircularBuffer() {
    close();
    // m_buffer will be freed by its own deleter
}

qint64 CircularBuffer::bytesAvailable() const {
    return m_buffer.size() + QIODevice::bytesAvailable();
}

bool CircularBuffer::canReadLine() const {
    return std::find(m_buffer.cbegin(), m_buffer.cend(), '\n') !=
               m_buffer.cend() ||
           QIODevice::canReadLine();
}

void CircularBuffer::clear() {
    m_buffer.clear();
    emit readChannelFinished();
}

void CircularBuffer::close() {
    if (isOpen()) {
        QIODevice::close();
        clear();
    }
}

bool CircularBuffer::isSequential() const {
    return true;
}

CircularBuffer::size_type CircularBuffer::capacity() const {
    return m_buffer.capacity();
}

bool CircularBuffer::reset() {
    m_buffer.clear();
    return QIODevice::reset();
}

#define CIRCULARBUFFER_SETCAPACITY_ERROR                                       \
    "Cannot set/change capacity while device is open."

void CircularBuffer::setCapacity(size_type cap, bool *ok = nullptr) {
    if (isOpen()) {
        setErrorString(QStringLiteral(CIRCULARBUFFER_SETCAPACITY_ERROR));
        qWarning() << QObject::tr(CIRCULARBUFFER_SETCAPACITY_ERROR);
        if (ok)
            *ok = false;
        return;
    }
    m_buffer.set_capacity(cap);
    if (ok)
        *ok = true;
}

qint64 CircularBuffer::readData(char *data, qint64 maxlen) {
    if (maxlen <= 0 || m_buffer.empty())
        return 0;

    const auto bytesToRead = qMin(size_type(maxlen), m_buffer.size());
    qint64 bytesRead = 0;

    // Read from the current readPos to the end of array (range) one
    auto range = m_buffer.array_one();
    const auto readToEnd = qMin(range.second, bytesToRead);
    if (readToEnd > 0) {
        std::copy_n(range.first, readToEnd, data);
        bytesRead += readToEnd;
    }

    // If needed, read from the beginning of array (range) two
    if (bytesRead < bytesToRead) {
        const auto remainingBytes = bytesToRead - bytesRead;
        range = m_buffer.array_two();
        Q_ASSERT(remainingBytes <= range.second);
        std::copy_n(range.first, remainingBytes, data + bytesRead);
        bytesRead += remainingBytes;
    }

    Q_ASSERT(bytesRead <= bytesToRead);
    m_buffer.erase_begin(bytesRead);
    // emit readyWrite();
    return bytesRead;
}

qint64 CircularBuffer::writeData(const char *data, qint64 len) {
    if (len <= 0)
        return 0;
    m_buffer.insert(m_buffer.end(), data, data + len);
    // m_writePos = (m_writePos + len) % m_buffer.capacity();
    emit readyRead();
    return len;
}

bool CircularBuffer::isErrorStringLocalizable() const {
    return errorString().compare(
        QLatin1StringView(CIRCULARBUFFER_SETCAPACITY_ERROR));
}
} // namespace VvvfSimulator::Generation::Util

#undef CIRCULARBUFFER_SETCAPACITY_ERROR
