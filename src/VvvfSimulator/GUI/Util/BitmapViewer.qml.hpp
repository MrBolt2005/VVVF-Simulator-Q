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
		Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
		Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
		Q_PROPERTY(bool isVisible READ isVisible WRITE setVisible NOTIFY isVisibleChanged)

		QPixmap m_pixmap;
		QString m_title;
		int m_width, m_height;
		bool m_isVisible = false;

		static constexpr size_t m_sp = sizeof(m_pixmap);
		static constexpr size_t m_st = sizeof(m_title);
		static constexpr size_t m_ss = sizeof(m_size);
		const size_t m_s = sizeof(*this);

	public:
		explicit BitmapViewer(
			const QPixmap &pixmap = QPixmap(),
			const QString &title = QObject::tr("Bitmap Viewer"),
			const QSize &size = QSize(450, 800),
			bool show = false,
			QObject *parent = nullptr
		);
		~BitmapViewer() override;

		// Getters
		QPixmap pixmap() const { return m_pixmap; }
		QString windowTitle() const noexcept { return m_title; }
		constexpr bool isVisible() const noexcept { return m_isVisible; }
		constexpr int width() const noexcept { return m_width; }
		constexpr int height() const noexcept { return m_height; }
		constexpr QSize size() const noexcept { return QSize(m_width, m_height); }
	//	constexpr const QSize &sizeRef() const noexcept { return m_size; }

	signals: //void requestResize(int width, int height);
		// Change Notification Signals	
		void heightChanged(int arg);
		void isVisibleChanged(bool arg);
		void pixmapChanged(const QPixmap &pixmap);
		void widthChanged(int arg);
		void windowTitleChanged(const QString &title);

	public slots:
		// Setters
		void close() { return setVisible(false); }
		void setHeight(int height) noexcept;
		void setPixmap(const QPixmap &pixmap);
		void setSize(const QSize &size);
		void setVisible(bool visible) noexcept;
		void setWidth(int width) noexcept;
		void setWindowTitle(const QString &title) noexcept;	
		void show()  { return setVisible(true); }
	};
} // namespace VvvfSimulator::GUI::Util