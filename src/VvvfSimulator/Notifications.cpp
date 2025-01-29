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
		QReadLocker(&m_lock);
		return -1;
	}

	void NotificationInterface::setTime(int time)
	{
		QWriteLocker(&m_lock);
		Q_UNUSED(time);
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

	int PushNotification::time() const
	{
		QReadLocker(&m_lock);
		return m_time;
	}

	void PushNotification::setTime(int time)
	{
		QWriteLocker(&m_lock);
		m_time = time;
	}

	void send(const QString& message) override
	{
		// Generic implementation for mobile platforms using QPushNotification
		QPushNotification notification;
		notification.setBody(message);
		notification.show();
		QObject::connect(&notification, &QPushNotification::clicked, this, &MobileNotification::notificationOpened);
	}

	#else

	#endif
}