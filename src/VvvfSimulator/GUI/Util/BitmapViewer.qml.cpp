#include "BitmapViewer.qml.hpp"

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

// BitmapViewer.qml.cpp
// Version 1.9.1.1

namespace VvvfSimulator::GUI::Util
{
	BitmapViewer::BitmapViewer(
		const QPixmap &pixmap,
		const QString &title,
		const QSize &size,
		bool show,
		QObject *parent
	) :
		QObject(parent),
		m_pixmap(pixmap),
		m_title(title),
		m_width(size.width()),
		m_height(size.height()),
		m_isVisible(show)
	{
	}

	BitmapViewer::~BitmapViewer() = default;

	void BitmapViewer::setHeight(int height) noexcept
	{
		if (m_size.height() != height)
		{
			m_height = height;
			emit heightChanged(m_height);
		}
	}

	void BitmapViewer::setPixmap(const QPixmap &pixmap)
	{
	if (m_pixmap.toImage() != pixmap.toImage())
		{
			m_pixmap = pixmap;
			emit pixmapChanged(m_pixmap);
		}
	}

	void BitmapViewer::setSize(const QSize &size)
	{
		setWidth(size.width());
		setHeight(size.height());
	}

	void BitmapViewer::setVisible(bool visible) noexcept
	{
		if (m_isVisible != visible)
		{
			m_isVisible = visible;
			emit isVisibleChanged(m_isVisible);
		}
	}

	void BitmapViewer::setWidth(int width) noexcept
	{
		if (m_size.width() != width)
		{
			m_width = width;
			emit widthChanged(m_width);
		}
	}
	
	void BitmapViewer::setWindowTitle(const QString &title) noexcept
	{
		if (m_title != title)
		{
			m_title = title;
			emit windowTitleChanged(m_title);
		}
	}
}