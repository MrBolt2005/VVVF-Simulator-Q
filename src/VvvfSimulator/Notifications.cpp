#include "Notifications.hpp"

#include <QReadLocker>
#include <QWriteLocker>

namespace VvvfSimulator
{
	QString NotificationInterface::title() const
	{
		QReadLocker locker(&m_lock);
		return m_title;
	}

	void NotificationInterface::setTitle(const QString& title)
	{
		QWriteLocker locker(&m_lock);
		m_title = title;
	}
	
	QString NotificationInterface::msg() const
	{
		QReadLocker locker(&m_lock);
		return m_msg;
	}

	void NotificationInterface::setMsg(const QString& message)
	{
		QWriteLocker locker(&m_lock);
		m_msg = message;
	}

	int NotificationInterface::time() const
	{
		QReadLocker locker(&m_lock);
		return m_time;
	}

	void NotificationInterface::setTime(int time)
	{
		QWriteLocker locker(&m_lock);
		m_time = time;
	}

	#ifdef __ANDROID__ || __VVVF_IOS__

	PushNotification::PushNotification(const QString& title, const QString& message, int time, bool sendOnCreation, QObject *parent)
	{
		{
			QWriteLocker locker(&m_lock);
			QObject::setParent(parent);
			m_title = title;
			m_msg = message;
			m_time = time;
		}
		if (sendOnCreation) send();
	}

	#else

	#endif
}