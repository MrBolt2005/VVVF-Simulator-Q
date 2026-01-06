/*
   Copyright © 2025 VvvfGeeks, VVVF Systems
   
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

// Standard Library Includes
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <locale>
#include <optional>
#include <string>
// Package Includes
#include <avcpp/av.h>

#include <boost/nowide/iostream.hpp>

#include <QCommandLineParser>
#include <QDebug>
#include <QGuiApplication>
#include <QLatin1StringView>
#include <QMessageBox>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QSharedMemory>
#include <QString>
#include <QtSystemDetection>
// Internal
#include "VvvfSimulator/Exception.hpp"

std::optional<std::filesystem::path> logPath = std::nullopt;
QtMessageHandler originalHandler = nullptr;
extern const char *aboutText, *helpText;

static void VvvfSimulator__logToFile(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	if (logPath.has_value())
	{
		const auto log = qFormatLogMessage(type, context, msg);
		static std::FILE *logFile = std::fopen(logPath->string().c_str(), "a, ccs=UTF-8");
		if (logFile)
		{
			std::fprintf(logFile, "%s\n", qUtf8Printable(msg));
			std::fflush(logFile);
			std::fclose(logFile);
		}
		else if (originalHandler) (*originalHandler)(
			QtWarningMsg,
			QMessageLogContext(__FILE__, __LINE__, Q_FUNC_INFO, "Runtime Error"),
			QObject::tr("Could not open the intended log file: %1").arg(logPath->u16string())
		);
	}
	else if (originalHandler) (*originalHandler)(
		QtWarningMsg,
		QMessageLogContext(__FILE__, __LINE__, Q_FUNC_INFO, "Runtime Error"),
		QObject::tr("No log file path was provided.")
	);

	if (originalHandler) (*originalHandler)(type, context, msg);
}

static inline std::locale trySwitchLocaleToUnicode(const std::locale &locale)
{
	const auto localeName = locale.name();
	// Strip encoding part from locale name if it exists
	const auto baseLocaleName = localeName.substr(0, localeName.find('.'));

	// Try to create the locale with ".UTF-8" suffix
	try { return std::locale(baseLocaleName + ".UTF-8"); }
	catch (const std::runtime_error&)
	{
		try { return std::locale(baseLocaleName + ".utf8"); }
		catch (const std::runtime_error&) { return locale; }
	}
}

int main(int argc, char *argv[])
{
	/*
	const char locUtf8[] = "UTF-8";
	char *loc = std::setlocale(LC_ALL, NULL);
	qDebug() << QObject::tr("Initial C locale: ") << (loc != nullptr ? loc : QObject::tr("( WARNING: Locale value could not be defined )")) << '\n';
	
	loc = std::setlocale(LC_ALL, locUtf8);
	if (loc == nullptr)
		qWarning() << QObject::tr("Could not define new C locale to: ") << locUtf8 << '\n';
	else if (std::strcmp(loc, locUtf8) != 0)
		qWarning() << QObject::tr("Could not define new C locale to: ") << locUtf8 << QObject::tr("; it was set instead to: ") << loc << '\n';
	else
		qDebug() << QObject::tr("Succesfully defined new C locale: ") << loc << '\n';
		*/

	av::init();
	
	// Set up standard output to UTF-8
	const auto originalCoutLocale = std::cout.getloc();
	auto newLocale = trySwitchLocaleToUnicode(originalCoutLocale);
	if (newLocale != originalCoutLocale) std::cout.imbue(newLocale);
	const auto originalWCoutLocale = std::wcout.getloc();
	     newLocale = trySwitchLocaleToUnicode(originalWCoutLocale);
	if (newLocale != originalWCoutLocale) std::wcout.imbue(newLocale);

	const auto VvvfOrganizationName = QStringLiteral("VvvfGeeks");
	//const auto VvvfApplicationName  = QStringLiteral("VVVF-Simulator");
	
	// Start up the Qt application
	QGuiApplication app(argc, argv);
	app.setOrganizationName(QStringLiteral("VVVF Systems"));
	app.setOrganizationDomain(QStringLiteral("vvvfgeeks.org"));
	app.setApplicationName(QStringLiteral("VVVF-Simulator-Q"));
	app.setApplicationDisplayName(QObject::tr("VVVF-Simulator"));
	app.setApplicationVersion(QStringLiteral("1.9.1.1"));

	//
	// Parse needed command line arguments
	//
	QCommandLineParser parser;
	parser.process(app);

	std::optional<QSettings> appSettings;
	// Get settings from default source or from file
	if (parser.isSet(QStringLiteral("settings")))
	{
		const auto settingsPath = parser.value(QStringLiteral("settings"));
		#if    defined(Q_OS_APPLE)
		appSettings.emplace(settingsPath, settingsPath.endsWith(QLatin1StringView(".ini"), Qt::CaseInsensitive) ? QSettings::IniFormat : QSettings::NativeFormat);
		#elif  defined(Q_OS_WIN)
		appSettings.emplace(settingsPath, settingsPath.startsWith(QLatin1StringView("HKEY")) ? QSettings::NativeFormat : QSettings::IniFormat);
		#else  // Linux et al.
		appSettings.emplace(settingsPath, QSettings::NativeFormat); // Equivalent to QSettings::IniFormat!
		#endif
	}
	else appSettings.emplace(QSettings::UserScope, app.organizationName(), app.applicationName());
	
	// Change settings
	
	const bool exitAfterCLI = !parser.isSet(QStringLiteral("no-exit-after-cli"));
	      bool wasCLIOutput = false;
	// Show help
	if (parser.isSet(QStringLiteral("help")))
	{
		const auto out = QObject::tr(helpText).toUtf8();
		boost::nowide::cout.write(out.constData(), out.size() + 1);
		wasCLIOutput = true;
	}
	// Show About text
	if (parser.isSet(QStringLiteral("about")))
	{
		const auto out = QObject::tr(aboutText).toUtf8();
		boost::nowide::cout.write(out.constData(), out.size() + 1);
		wasCLIOutput = true;
	}

	// Log to file
	if (parser.isSet(QStringLiteral("log-to-file")))
	{
		const std::filesystem::path path(parser.value(QStringLiteral("log-to-file")).toStdU16String());
		if (path.empty()) qWarning() << QObject::tr("Command line option --log-to-file: Could not set, the provided path was empty.");
		else
		{
			logPath.emplace(parser.value(QStringLiteral("log-to-file")));
			originalHandler = qInstallMessageHandler(VvvfSimulator__logToFile);
		}
	}

	if (exitAfterCLI && wasCLIOutput) return 1;

	// Check if an instance is already running
	QSharedMemory checkInstances;
	checkInstances.setKey(QStringLiteral("%1_v%2_checkInstances").arg(app.applicationName()).arg(app.applicationVersion()));

	if (!checkInstances.create(1) && !parser.isSet(QStringLiteral("ignore-instance-check"))) {
		// Another instance is already running
		QMessageBox msgBox;
		msgBox.setText(QObject::tr("Another instance of this application is already running."));
		msgBox.setInformativeText(QObject::tr("Do you want to continue launching it?"));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);

		const auto check = msgBox.exec();
		if (check == QMessageBox::No) {
				// User chose not to launch another instance
				return check;
		}
	}

	// Set up the QML engine
	QQmlApplicationEngine engine(&app);

	// Set up the QML context

	const auto ret = app.exec();

	// Revert cout locale
	if (std::cout.flush().getloc()  != originalCoutLocale)  std::cout.imbue(originalCoutLocale);
	if (std::wcout.flush().getloc() != originalWCoutLocale) std::wcout.imbue(originalWCoutLocale);

	return ret;
}
