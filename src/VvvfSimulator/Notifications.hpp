#include <QReadWriteLock>
#include <QObject>
#include <QString>

#if defined(__ANDROID__) || defined(__VVVF_IOS__)
#define VVVF_PUSH_NOTIFICATION
#include <QPushNotification>
#else
#include <QAtomicInt>
#include <memory>
//#include <QSystemTrayIcon>
#endif // __ANDROID__ || __VVVF_IOS__

namespace VvvfSimulator
{
	class Notification : public QObject
	{
		Q_OBJECT

		mutable QReadWriteLock m_lock;
		QString m_title, m_msg;
	
	#ifndef VVVF_PUSH_NOTIFICATION
		QAtomicInt m_time;
		std::unique_ptr<QSystemTrayIcon> m_trayIcon;
	#endif//VVVF_PUSH_NOTIFICATION
		
	public:
		Notification(const QString& title, const QString& message, bool sendOnCreation = true, int time = 10000, QObject *parent = nullptr);
		~Notification() override;
		void send();
		QString title() const;
		void setTitle(const QString& title);
		QString msg() const;
		void setMsg(const QString& message);
		int time() const;
		void setTime(int time);

	signals:
		void notificationOpened();
	};
}
