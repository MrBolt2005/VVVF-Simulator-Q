#include "Notifications.hpp"

#include <QReadLocker>
#include <QWriteLocker>

namespace VvvfSimulator
{
	Notification::Notification(const QString& title, const QString& message, bool sendOnCreation, int time, QObject *parent)
	{
		{
			QWriteLocker locker(&m_lock);
			m_title = title;
			m_msg = message;
			#ifndef VVVF_PUSH_NOTIFICATION
			m_time = time;
			m_trayIcon = std::move(std::unique_ptr(new QSystemTrayIcon(QIcon::fromTheme("dialog-information"))));
			#else
			Q_UNUSED(time);
			#endif
		}
		if (sendOnCreation) send();
	}

	Notification::~Notification()
	{
		QWriteLocker locker(&m_lock);
	}

	void Notification::send()
	{
		#ifdef VVVF_PUSH_NOTIFICATION
		// Implementation for push notifications
		QPushNotification notification;
		notification.setBody(message);
		notification.show();
		QObject::connect(&notification, &QPushNotification::clicked, this, &Notification::notificationOpened);
		#else
		// Implementation for tray icons
		trayIcon->show();
		trayIcon->showMessage(title, message, QSystemTrayIcon::Information, time);
		QObject::connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &Notification::notificationOpened);
		#endif // VVVF_PUSH_NOTIFICATION
	}

	QString Notification::title() const
	{
		QReadLocker locker(&m_lock);
		return m_title;
	}

	void Notification::setTitle(const QString& title)
	{
		QWriteLocker locker(&m_lock);
		m_title = title;
	}

	QString Notification::msg() const
	{
		QReadLocker locker(&m_lock);
		return m_msg;
	}

	void Notification::setMsg(const QString& message)
	{
		QWriteLocker locker(&m_lock);
		m_msg = message;
	}
}