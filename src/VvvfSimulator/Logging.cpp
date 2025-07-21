#include "Logging.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QObject>

void VvvfSimulator::Logging::consoleAndMsgBoxLog(const QString &msg, const QMessageLogContext &context, QtMsgType type, QWidget *parent)
{
	// Prepare the context information
	const QString contextInfo = QString("[%1:%2, %3]").arg(context.file).arg(context.line).arg(context.function);

	// Log to console with context information
	// Show message box corresponding to the message type
	switch (type)
	{
	case QtDebugMsg:
		qDebug() << contextInfo << msg;
		break;
	case QtInfoMsg:
		qInfo() << contextInfo << msg;
		QMessageBox::information(parent, QObject::tr("Information"), msg);
		break;
  case QtWarningMsg:
		qWarning() << contextInfo << msg;
		QMessageBox::warning(parent, QObject::tr("Error Warning"), msg);
		break;
  case QtCriticalMsg:
		qCritical() << contextInfo << msg;
		QMessageBox::critical(parent, QObject::tr("Critical Error"), msg);
		break;
  case QtFatalMsg:
		// We need to log to the message box first here, because qFatal() will terminate the application	
		QMessageBox::critical(nullptr, QObject::tr("Fatal Error"), msg);
		qFatal("%s %s", qPrintable(contextInfo), qPrintable(msg));
		break;
	}
}