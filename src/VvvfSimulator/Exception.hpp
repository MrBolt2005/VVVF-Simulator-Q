#include <stdexcept>

#include <QException>
#include <QVariantMap>

#define QEXCEPTION_COMMON_BODY_DECL    \
public:                                \
	virtual void raise() const;        \
    virtual QException *clone() const; \
private:                               \

namespace VvvfSimulator::Exception
{
	class ConstructorError : public QException
	{
		QEXCEPTION_COMMON_BODY_DECL
		
		QString m_msg;
		int m_errno, m_lineNumber;
		QVariantMap m_ctx;

		public:
			ConstructorError() = default;
			ConstructorError(const QString& message, int errorCode);
	};
}