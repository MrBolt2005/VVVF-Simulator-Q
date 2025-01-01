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
#include <clocale>
#include <cstring>
// Package Includes
#include <QDebug>
#include <QGuiApplication>
#include <QObject>
#include <QQmlApplicationEngine>

int main(int argv, char *argv[])
{
	const char locUtf8[] = "UTF-8";
	char *loc = std::setlocale(LC_ALL, NULL);
	qDebug() << QObject::tr("Initial C locale: ") << loc != nullptr ? loc : QObject::tr("( WARNING: Locale value could not be defined )") << '\n';
	
	loc = std::setlocale(LC_ALL, locUtf8);
	if (loc == nullptr)
		qWarning() << QObject::tr("Could not define new C locale to: ") << locUtf8 << '\n';
	else if (std::strcmp(loc, locUtf8) != 0)
		qWarning() << QObject::tr("Could not define new C locale to: ") << locUtf8 << QObject::tr("; it was set instead to: ") << loc << '\n';
	else
		qDebug() << QObject::tr("Succesfully defined new C locale: ") << loc << '\n';
	// Start up Qt Framework
}
