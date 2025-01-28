#include <QAtomicInt>
#include <QReadWriteLock>
#include <QScopedPointer>
#include <QString>
#include <QObject>

#ifdef __ANDROID__ || __VVVF_IOS__
#include <QPushNotification>
#else
#include <QSystemTrayIcon>
#endif // __ANDROID__ || __VVVF_IOS__

namespace VvvfSimulator
{
	// Cross-platform push notifications
	// Base interface
	class NotificationInterface : public QObject
	{
		Q_OBJECT

        NotificationInterface();

	protected:
		mutable QReadWriteLock m_lock;
		QString m_title, m_msg;
		QAtomicInt m_time;

	public:
		//virtual NotificationInterface(const QString *message = nullptr, bool sendOnCreation = true, QObject *parent = nullptr) = 0;
		~NotificationInterface() override = default;
		virtual void send() = 0;
		virtual QString title() const;
		virtual void setTitle(const QString& title);
		virtual QString msg() const;
		virtual void setMsg(const QString& message);
        virtual int time() const;
        virtual void setTime(int time);

	signals:
		void notificationOpened();
	};
	#ifdef __ANDROID__ || __VVVF_IOS__
	
	class PushNotification : public NotificationInterface
	{
		Q_OBJECT
	protected:
		int m_time;
	
	public:
		PushNotification(const QString& title, const QString& message, bool sendOnCreation = true, QObject *parent = nullptr);
		constexpr ~PushNotification() override = default;
		void send() override;
	};

	using Notification = PushNotification;

	#else

	class TrayIconNotification : public NotificationInterface
	{
		Q_OBJECT
	protected:
		int m_time;

	private:
		QScopedPointer<QSystemTrayIcon> m_trayIcon;

	public:
		TrayIconNotification(const QString& title, const QString& message, bool sendOnCreation = true, QObject *parent = nullptr);
		constexpr ~TrayIconNotification() override = default;
		void send() override;
	};

	using Notification = TrayIconNotification;

	#endif
}