#include "Notifications.hpp"
#include <QMutexLocker>

namespace VvvfSimulator
{
	QString NotificationInterface::msg() const
	{
		QMutexLocker locker(&m_lock);
		return m_msg;
	}

	bool NotificationInterface::setMsg(const QString& message)
	{
		if (m_lock.try_lock())
		{
			m_msg = message;
			m_lock.unlock();
			return true;
		}
		else return false;
	}
}