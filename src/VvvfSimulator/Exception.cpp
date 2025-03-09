#include "Exception.hpp"
#include "Logging.hpp"

namespace VvvfSimulator::Exception
{
	QEXCEPTION_COMMON_BODY_IMPL(VvvfException)
	
	VvvfException::VvvfException() : m_dateTimeStamp(QDateTime::currentDateTime()) {}

	VvvfException::VvvfException(
		const char* message,
		const QMessageLogContext *context,
		const QVariantMap *variantMap,
		const std::exception_ptr& exceptionPtr,
		qsizetype messageSize,
		const QDateTime* dateTimeStamp) :
		m_dateTimeStamp(dateTimeStamp ? *dateTimeStamp : QDateTime::currentDateTime()),
		m_exceptionPtr(exceptionPtr),
		m_msg(message, messageSize),
		// We cannot use QMessageLogContext's copy constructor because it is deleted:
		m_ctx(context ? QMessageLogContext(context->file, context->line, context->function, context->category) : QMessageLogContext()),
		m_variantMap(variantMap ? *variantMap : QVariantMap())
	{}

	VvvfException::VvvfException(const VvvfException& other) :
		m_dateTimeStamp(other.m_dateTimeStamp),	
		m_exceptionPtr(other.m_exceptionPtr),
		m_msg(other.m_msg),
  	// We cannot use QMessageLogContext's copy constructor because it is deleted:
		m_ctx(other.m_ctx.file, other.m_ctx.line, other.m_ctx.function, other.m_ctx.category),
		m_variantMap(other.m_variantMap)
  {}

	VvvfException::~VvvfException() noexcept = default;

	const char* VvvfException::what() const noexcept
	{
		return m_msg.constData();
	}
}