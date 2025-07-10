#pragma once

// Standard Library
#include <cstdlib>
#include <memory>
// Packages
#include <QIODevice>

namespace VvvfSimulator::Generation::Util
{
	class CircularBuffer : public QIODevice
	{
		Q_OBJECT

	public:
		typedef std::ptrdiff_t size_type;
		Q_PROPERTY(size_type maxSize READ maxSize WRITE setMaxSize)
		Q_PROPERTY(size_type capacity READ capacity)
		
		explicit CircularBuffer(size_type maxSize = 16384, QObject *parent = nullptr);
		~CircularBuffer() override;

		// QIODevice interface implementation
		void close() override;
		bool isSequential() const override;
		//bool open(OpenMode mode) override;
		bool reset() override;
		
		// Buffer specific methods
		bool canReadLine() const override;
		void clear();
		constexpr size_type maxSize() const noexcept { return m_maxSize; }
		qint64 bytesAvailable() const override;
		//qint64 bytesToWrite() const override;
		
		// Buffer capacity management
		constexpr size_type capacity() const noexcept { return m_cap; }

	public slots:
		void setMaxSize(size_type maxSize);
		void squeeze(); // New method to shrink the buffer to optimal size
		void shrink_to_fit() { return squeeze(); }

	protected:
		qint64 readData(char *data, qint64 maxlen) override;
		qint64 writeData(const char *data, qint64 len) override;

	private:
		std::unique_ptr<char[], void(*)(void*)> m_buffer;
		size_type m_cap;       // Current buffer capacity
		size_type m_readPos;
		size_type m_writePos;
		size_type m_byteCount;
		size_type m_maxSize;   // Maximum allowed size

		friend struct CircularBufferPrivate; // To not need to declare outline private methods here...
	};
}