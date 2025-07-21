#pragma once

// Standard Library
#include <atomic>
#include <mutex>
// Packages
#include <QDebug>
// Internal
#include "String.hpp"

namespace VvvfSimulator::Util::Thread
{
	// @brief
	// A hacky way to use and lock mutexes in a class while still keeping the
	// method const is by having the mutex member be accessed indirectly via a
	// pointer, since even by doing a non-const operation to the pointer's
	// dereference, the pointer itself still remains unchanged.\n
	// Since neither the STL or Qt will let us make a smart pointer out of a
	// mutex, this class allows the indirected mutex to be handled with RAII and
	// auto-deletion after going out of scope while contouring this restriction.
	struct MutexAPtrWrapper
	{
		// Using std::mutex instead of QMutex because the latter already works with
		// a pointer-to-implementation, so making an unique_ptr to QMutex would add
		// an unnecessary 2nd pointer dereferencing operation...
		const std::atomic<std::mutex *> data;

		MutexAPtrWrapper() : data(new std::mutex()) {}

		MutexAPtrWrapper(std::mutex *toOwn)
			: data(toOwn ? toOwn : new std::mutex())
		{
		}

		~MutexAPtrWrapper()
		{
			auto x = data.load();
			if (x)
			{
				delete x;
				qDebug() << qTr("Deleted object member mutex at 0x%1")
					.arg(ptrdiff_t(x), 16);
			}
		}

		operator const decltype(data) &() const noexcept { return data; }
	};
}