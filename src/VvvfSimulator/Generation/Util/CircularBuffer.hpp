#pragma once

#ifndef GENERATION__UTIL__CIRCULARBUFFER_HPP
#define GENERATION__UTIL__CIRCULARBUFFER_HPP

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Generation/Util/CircularBuffer.hpp
 * v1.10.0.1
 */

// Packages
#include <boost/circular_buffer.hpp>
#include <QIODevice>

namespace VvvfSimulator::Generation::Util {
class CircularBuffer : public QIODevice {
    Q_OBJECT

  public:
    typedef std::size_t size_type;
    Q_PROPERTY(size_type capacity READ capacity WRITE setCapacity)

    explicit CircularBuffer(size_type capacity = 16384,
                            QObject *parent = nullptr);
    ~CircularBuffer() override;

    // QIODevice interface implementation
    qint64 bytesAvailable() const override;
    bool canReadLine() const override;
    void close() override;
    bool isSequential() const override;
    bool reset() override;

    // Setters / Getters
    size_type capacity() const;
    bool
    setCapacity(size_type cap); // Returns the amount of deleted elements if any

    void clear();

  protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

    virtual bool isErrorStringLocalizable() const;

  private:
    boost::circular_buffer<char> m_buffer;
    // size_type m_readPos, m_writePos;
};
} // namespace VvvfSimulator::Generation::Util

#endif // GENERATION__UTIL__CIRCULARBUFFER_HPP
