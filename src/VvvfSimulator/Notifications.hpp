#include <QMutex>
#include <QRunnable>
#include <QScopedPointer>
#include <QString>
#include <QSystemTrayIcon>
#include <QObject>

namespace VvvfSimulator
{
	// Cross-platform push notifications
	// Base interface
	class NotificationInterface : public QObject
	{
		Q_OBJECT

		QScopedPointer<QRunnable> onOpenCallback;

	public:		
		QString msg;

		//virtual NotificationInterface(const QString *message = nullptr, bool sendOnCreation = true, QObject *parent = nullptr) = 0;
		constexpr ~NotificationInterface() override = default;
		virtual void send() = 0;
	};
	#ifdef __ANDROID__ || __VVVF_IOS__
	
	class PushNotification : public NotificationInterface
	{
	public:
		PushNotification(const QString *message = nullptr, bool sendOnCreation = true, QObject *parent = nullptr);
		constexpr ~PushNotification() override = default;
		void send() override;
	};

	#define VVVF_NOTIFICATION PushNotification;

	#else

	class TrayIconNotification : public NotificationInterface
	{
		QScopedPointer<QSystemTrayIcon> m_trayIcon;

	public:
		TrayIconNotification(const QString *message = nullptr, bool sendOnCreation = true, QObject *parent = nullptr);
		constexpr ~TrayIconNotification() override = default;
		void send() override;
	};

	#define VVVF_NOTIFICATION TrayIconNotification;

	#endif
}