#include "CircularBuffer.hpp"

// Standard Library
#include <algorithm>
#include <cstring>
#include <new>
// Packages
#include <QDebug>
#include <QtGlobal>
// Internal
#include "../../Util/Defines.h"

// Private stuff

MAYBE_PUSH(INITIAL_SIZE)
#define INITIAL_SIZE CircularBufferPrivate(*this).initialSize()

MAYBE_PUSH(REORGANIZE_BUFFER)
#define REORGANIZE_BUFFER CircularBufferPrivate(*this).reorganizeBuffer()

MAYBE_PUSH(REALLOCATE_BUFFER)
#define REALLOCATE_BUFFER(newCapacity) CircularBufferPrivate(*this).reallocateBuffer(newCapacity)

MAYBE_PUSH(INITIALIZE_BUFFER)
#define INITIALIZE_BUFFER CircularBufferPrivate(*this).initializeBuffer()
namespace VvvfSimulator::Generation::Util
{
	struct CircularBufferPrivate
	{
		CircularBuffer *target;
		constexpr CircularBufferPrivate(CircularBuffer &target) noexcept
			: target(&target)
		{}

		using size_type = CircularBuffer::size_type;

		constexpr size_type initialSize() const
		{
			// Find the greatest odd factor of maxSize
			auto size = target->m_maxSize;
			
			// Handle edge case
			if (size <= 0)
				return 1;
			
			// Find the greatest odd factor
			while (size > 0 && !(size & 1))
				size >>= 1;
			
			return qMax<size_type>(1, size);
		}

		void reorganizeBuffer()
		{
			// Only reorganize if there's data and it wraps around
			if (target->m_byteCount > 0 && target->m_readPos > target->m_writePos)
			{
				// Create a temporary buffer to hold all data in correct order
				std::unique_ptr<char[]> tempBuf(new char[target->m_byteCount]);
				
				// Read data into temp buffer
				const auto firstPart = target->m_cap - target->m_readPos;
				memcpy(tempBuf.get(), target->m_buffer.get() + target->m_readPos, firstPart);
				
				if (target->m_writePos > 0)
					memcpy(tempBuf.get() + firstPart, target->m_buffer.get(), target->m_writePos);
				
				// Copy back to main buffer at the beginning
				memcpy(target->m_buffer.get(), tempBuf.get(), target->m_byteCount);
				
				// Update positions
				target->m_readPos = 0;
				target->m_writePos = target->m_byteCount;
			}
		}

		void reallocateBuffer(size_type newCapacity)
		{
			if (newCapacity <= 0 || newCapacity == target->m_cap)
				return;

			// If data wraps around, reorganize it first 
			// (only needed when shrinking or when growing will change the data layout)
			if (target->m_byteCount > 0 && target->m_readPos > target->m_writePos)
				REORGANIZE_BUFFER;
			
			// Allocate new buffer
			const auto newMem = (char*)std::realloc(
				target->m_buffer.release(),
				static_cast<size_t>(newCapacity)
			);
			if (!newMem)
				throw std::bad_alloc();
			
			// Update unique_ptr with new buffer
			target->m_buffer.reset(newMem);
			
			// Update capacity
			target->m_cap = newCapacity;
			
			// Make sure positions are still valid in the new buffer
			target->m_readPos = qMin(target->m_readPos, target->m_cap - 1);
			target->m_writePos = qMin(target->m_writePos, target->m_cap - 1);
			
			// Make sure byteCount doesn't exceed buffer capacity
			target->m_byteCount = qMin(target->m_byteCount, target->m_cap);
		}

		void initializeBuffer()
		{
			const auto initialCapacity = initialSize();
			
			// Allocate the initial buffer
			const auto mem = (char*)std::calloc(static_cast<size_t>(initialCapacity), 1);
			if (!mem)
				throw std::bad_alloc();
			
			target->m_buffer.reset(mem);
			target->m_cap = initialCapacity;
			target->m_readPos = 0;
			target->m_writePos = 0;
			target->m_byteCount = 0;
		}
	};

	// End of private stuff

	CircularBuffer::CircularBuffer(size_type maxSize, QObject *parent)
		: QIODevice(parent)
		, m_buffer(nullptr, std::free)
		// Postpone initialization to initializeBuffer()
		/*
		, m_cap(0)
		, m_readPos(0)
		, m_writePos(0)
		, m_byteCount(0)
		*/
		, m_maxSize(maxSize > 0 ? maxSize : 16384)
	{
		INITIALIZE_BUFFER;
	}

	CircularBuffer::~CircularBuffer()
	{
		if (isOpen())
		{
			QIODevice::close();
			clear();
		}
		// m_buffer will be freed by unique_ptr's deleter
	}

	qint64 CircularBuffer::bytesAvailable() const
	{
		return m_byteCount + QIODevice::bytesAvailable();
	}

	bool CircularBuffer::canReadLine() const
	{
		if (const auto x = QIODevice::canReadLine(); x)
			return x;
		// else
		if (m_byteCount == 0)
			return false;
			
		// Search for newline character
		auto count = m_byteCount;
		auto pos = m_readPos;
		
		while (count > 0)
		{
			if (m_buffer[pos] == '\n')
				return true;
			pos = (pos + 1) % m_cap;
			count--;
		}
		
		return false;
	}

	void CircularBuffer::clear()
	{
		m_readPos = 0;
		m_writePos = 0;
		m_byteCount = 0;
		emit readChannelFinished();
	}

	void CircularBuffer::close()
	{
		if (isOpen())
		{
			QIODevice::close();
			clear();
		}
	}

	bool CircularBuffer::isSequential() const { return true; }

	qint64 CircularBuffer::readData(char *data, qint64 maxlen)
	{
		if (maxlen <= 0 || m_byteCount == 0)
			return 0;
			
		const auto bytesToRead = qMin(maxlen, m_byteCount);
		qint64 bytesRead = 0;
		
		// Read from the current read position to the end of the buffer
		const auto readToEnd = qMin(m_cap - m_readPos, bytesToRead);
		if (readToEnd > 0)
		{
			std::copy_n(m_buffer.get() + m_readPos, readToEnd, data);
			m_readPos = (m_readPos + readToEnd) % m_cap;
			bytesRead += readToEnd;
		}
		
		// If needed, read from the beginning of the buffer
		if (bytesRead < bytesToRead)
		{
			const auto remainingBytes = bytesToRead - bytesRead;
			std::copy_n(m_buffer.get(), remainingBytes, data + bytesRead);
			m_readPos = remainingBytes;
			bytesRead += remainingBytes;
		}
		
		m_byteCount -= bytesRead;
		
		return bytesRead;
	}


	bool CircularBuffer::reset()
	{
		m_readPos = 0;
		m_writePos = 0;
		m_byteCount = 0;
		return QIODevice::reset();
	}

	void CircularBuffer::setMaxSize(size_type maxSize)
	{
		const size_type oldMaxSize = m_maxSize;
		m_maxSize = qMax<size_type>(1, maxSize);
		
		// Handle capacity adjustments when max size changes
		if (m_maxSize != oldMaxSize)
		{
			// Case 1: New max size is smaller than current capacity
			if (m_maxSize < m_cap)
			{
				// Case 1a: Current data exceeds new max size, truncate data
				if (m_byteCount > m_maxSize)
				{
					// Truncate data to fit new max size
					m_byteCount = m_maxSize;
					m_writePos = (m_readPos + m_byteCount) % m_cap;
				}
				
				// Case 1b: Current capacity significantly exceeds new max size
				// (more than twice) - shrink buffer to save memory
				if (m_cap > 2 * m_maxSize)
				{
					auto newCapacity = qMax(m_byteCount * 2, INITIAL_SIZE);
					newCapacity = qMin(newCapacity, m_maxSize);
					REALLOCATE_BUFFER(newCapacity);
				}
			}
			// Case 2: New max size is larger, but we don't need to grow yet
			// We'll let the buffer grow as needed during write operations
		}
	}

	void CircularBuffer::squeeze()
	{
		// No need to squeeze if empty or already at optimal size
		if (m_byteCount == 0)
		{
			// If empty, reallocate to initial size
			const auto is = INITIAL_SIZE;
			if (m_cap > is) REALLOCATE_BUFFER(is);
		}
		
		// If data amount is much less than capacity, shrink to optimal size
		else if (m_cap > 2 * m_byteCount)
		{
			// Target size is twice the data size to allow some room for growth
			auto newCapacity = qMax(m_byteCount * 2, INITIAL_SIZE);
			newCapacity = qMin(newCapacity, m_maxSize);
			REALLOCATE_BUFFER(newCapacity);
		}
	}

	qint64 CircularBuffer::writeData(const char *data, qint64 len)
	{
		if (len <= 0)
			return 0;
			
		// Calculate how many bytes we can actually write
		const auto bytesToWrite = qMin(len, m_maxSize - m_byteCount);
		if (bytesToWrite <= 0)
			return 0;
		
		// Check if we need to grow the buffer
		const auto requiredCapacity = m_byteCount + bytesToWrite;
		if (requiredCapacity > m_cap)
		{
			// Use a doubling growth strategy, but cap at m_maxSize
			const auto newCapacity = qMin(m_maxSize, qMax(requiredCapacity, m_cap * 2));
			REALLOCATE_BUFFER(newCapacity);
		}
		
		qint64 bytesWritten = 0;
		
		// Write to the end of the buffer
		const auto writeToEnd = qMin(m_cap - m_writePos, bytesToWrite);
		if (writeToEnd > 0)
		{
			std::copy_n(data, writeToEnd, m_buffer.get() + m_writePos);
			m_writePos = (m_writePos + writeToEnd) % m_cap;
			bytesWritten += writeToEnd;
		}
		
		// If needed, write to the beginning of the buffer
		if (bytesWritten < bytesToWrite)
		{
			const auto remainingBytes = bytesToWrite - bytesWritten;
			std::copy_n(data + bytesWritten, remainingBytes, m_buffer.get());
			m_writePos = remainingBytes;
			bytesWritten += remainingBytes;
		}
		
		m_byteCount += bytesWritten;
		
		// Notify that data is available to read
		emit readyRead();
		
		return bytesWritten;
	}
}

// Cleanup
#undef INITIALIZE_BUFFER
#undef REALLOCATE_BUFFER
#undef REORGANIZE_BUFFER
#undef INITIAL_SIZE

MAYBE_POP(INITIALIZE_BUFFER)
MAYBE_POP(REALLOCATE_BUFFER)
MAYBE_POP(REORGANIZE_BUFFER)
MAYBE_POP(INITIAL_SIZE)
