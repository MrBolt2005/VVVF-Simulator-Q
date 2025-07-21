#pragma once

#include <optional>
#include <memory>
#include <stdexcept>

#include <QByteArray>
#include <QDateTime>
#include <QException>
#include <QMessageLogContext>
#include <QReadWriteLock>
#include <QVariantMap>
#include <QVector>

// Define a macro to make functions constexpr only if the current C++ version supports constexpr virtual methods
#if __cplusplus >= 202002L // C++20 or later
#define CONSTEXPR_VIRTUAL constexpr virtual
#else
#define CONSTEXPR_VIRTUAL virtual
#endif

#define QEXCEPTION_COMMON_BODY_DECL    \
public:                                \
    void raise() const override;        \
    QException *clone() const override; \
private:                               \

#define QEXCEPTION_COMMON_BODY_IMPL(CLASS) \
void CLASS::raise() const { throw *this; } \
QException *CLASS::clone() const { return new CLASS(*this); } \

namespace VvvfSimulator::Exception
{
	class VvvfException : public QException
	{
		QEXCEPTION_COMMON_BODY_DECL
        
	protected:
		QDateTime m_dateTimeStamp;
		std::exception_ptr m_exceptionPtr;
		QByteArray m_msg;
		QMessageLogContext m_ctx;
		QVariantMap m_variantMap;
		//std::optional<int> m_errorCode;

	public:
		VvvfException();
		VvvfException(
			const char* message,
			const QMessageLogContext *context = nullptr,
			const QVariantMap *variantMap = nullptr,
			const std::exception_ptr& exceptionPtr = nullptr,
			qsizetype messageSize = -1,
			const QDateTime* dateTimeStamp = nullptr
		);
		VvvfException(const VvvfException& other);
		~VvvfException() noexcept override;

		// This method is not thread-safe, because it does not lock the read-write lock. It still is reentrant, though.
		const char* what() const noexcept override;
		virtual QDateTime dateTimeStamp() const noexcept { return m_dateTimeStamp; }
		virtual std::exception_ptr exceptionPtr() const noexcept { return m_exceptionPtr; }
		virtual QByteArray messageString() const noexcept { return m_msg; }
		CONSTEXPR_VIRTUAL QMessageLogContext context() const noexcept
		{
			return QMessageLogContext(m_ctx.file, m_ctx.line, m_ctx.function, m_ctx.category);
		}
		virtual QVariantMap variantMap() const noexcept { return m_variantMap; }
  };
}