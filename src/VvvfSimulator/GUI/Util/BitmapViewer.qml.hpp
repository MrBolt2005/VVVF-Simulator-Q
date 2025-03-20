#pragma once

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

// BitmapViewer.qml.hpp
// Version 1.9.1.1

// Packages
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QString>

namespace VvvfSimulator::GUI::Util
{
	class BitmapViewer : public QObject
	{
		//using QPixmap = ::QPixmap; // Have to do this because it errors out for some reason otherwise
		
		Q_OBJECT
		Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged)
		Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

		QPixmap m_pixmap;
		QString m_title;

		static constexpr size_t m_sp = sizeof(m_pixmap);
		static constexpr size_t m_st = sizeof(m_title);
		const size_t m_s = sizeof(*this);

	public:
		explicit BitmapViewer(
			const QPixmap &pixmap = QPixmap(),
			const QString &title = QObject::tr("Bitmap Viewer"),
			const QSize *size = nullptr,
			QObject *parent = nullptr
		);
		~BitmapViewer() override;

		// Getters
		QPixmap pixmap() const { return m_pixmap; }
		QString title() const noexcept { return m_title; }

		// Setters
		void setPixmap(const QPixmap &pixmap);
		void setTitle(const QString &title) noexcept;

	signals:
		void pixmapChanged(const QPixmap &pixmap);
		void requestResize(int width, int height);
		void titleChanged(const QString &title);
		void windowResized(int width, int height);
	};
} // namespace VvvfSimulator::GUI::Util