#include <QMutex>
#include <QRunnable>
#include <QScopedPointer>
#include <QString>
#ifdef __ANDROID__ || __VVVF_IOS__
#include <QPushNotification>
#else
#include <QSystemTrayIcon>
#endif // __ANDROID__ || __VVVF_IOS__
#include <QObject>

namespace VvvfSimulator
{
	// Cross-platform push notifications
	// Base interface
	class NotificationInterface : public QObject
	{
		Q_OBJECT

		QMutex m_lock;
		QScopedPointer<QRunnable> m_onOpenCallback;
		QString m_msg;

	public:

		//virtual NotificationInterface(const QString *message = nullptr, bool sendOnCreation = true, QObject *parent = nullptr) = 0;
		constexpr ~NotificationInterface() override = default;
		virtual void send() = 0;
		virtual QString msg() const;
		virtual bool setMsg(const QString& message);
	};
	#ifdef __ANDROID__ || __VVVF_IOS__
	
	class PushNotification : public NotificationInterface
	{
	public:
		PushNotification(const QString *message = nullptr, bool sendOnCreation = true, QObject *parent = nullptr);
		constexpr ~PushNotification() override = default;
		void send() override;
	};

	using Notification = PushNotification;

	#else

	class TrayIconNotification : public NotificationInterface
	{
		QScopedPointer<QSystemTrayIcon> m_trayIcon;

	public:
		TrayIconNotification(const QString *message = nullptr, bool sendOnCreation = true, QObject *parent = nullptr);
		constexpr ~TrayIconNotification() override = default;
		void send() override;
	};

	using Notification = TrayIconNotification;

	#endif
}