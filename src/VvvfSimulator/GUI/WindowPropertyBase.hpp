#pragma once

// File: WindowPropertyBase.hpp
// Copyright ...
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
// Version: 1.9.1.1

// Standard Library
#include <optional>
// Packages
#include <QColor>
#include <QObject>
#include <QPoint>
#include <QQuickWindow>
#include <QScreen>
#include <QSize>
#include <QString>
#include <QVariantMap>

/*
	@brief Base class to bind window properties between the QML and C++ sides à la QWidgets' QWindow.
*/

namespace VvvfSimulator::GUI
{
	class WindowPropertyBase : public QObject
	{
		Q_OBJECT
		// Size properties
		Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
		Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)
		Q_PROPERTY(int minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumWidthChanged)
		Q_PROPERTY(int minimumHeight READ minimumHeight WRITE setMinimumHeight NOTIFY minimumHeightChanged)
		Q_PROPERTY(int maximumWidth READ maximumWidth WRITE setMaximumWidth NOTIFY maximumWidthChanged)
		Q_PROPERTY(int maximumHeight READ maximumHeight WRITE setMaximumHeight NOTIFY maximumHeightChanged)
		
		// Position properties
		Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
		Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
		
		// State properties
		Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
		Q_PROPERTY(int visibility READ visibility WRITE setVisibility NOTIFY visibilityChanged)
		Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
		Q_PROPERTY(bool fullScreen READ isFullScreen WRITE setFullScreen NOTIFY fullScreenChanged)
		
		// Appearance properties
		Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
		Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
		Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
		Q_PROPERTY(int flags READ flagsInt WRITE setFlagsInt NOTIFY flagsChangedInt)
		
		// Screen property
		Q_PROPERTY(QScreen* screen READ screen WRITE setScreen NOTIFY screenChanged)
		
		// Content properties
		Q_PROPERTY(Qt::ScreenOrientation contentOrientation READ contentOrientation WRITE setContentOrientation NOTIFY contentOrientationChanged)
		
		// Modality
		Q_PROPERTY(Qt::WindowModality modality READ modality WRITE setModality NOTIFY modalityChanged)
		
		// Rendering properties
		Q_PROPERTY(bool clearBeforeRendering READ clearBeforeRendering WRITE setClearBeforeRendering NOTIFY clearBeforeRenderingChanged)
		Q_PROPERTY(bool persistentOpenGLContext READ persistentOpenGLContext WRITE setPersistentOpenGLContext NOTIFY persistentOpenGLContextChanged)
		Q_PROPERTY(bool persistentSceneGraph READ persistentSceneGraph WRITE setPersistentSceneGraph NOTIFY persistentSceneGraphChanged)
		
		// Cursor property
		Q_PROPERTY(Qt::CursorShape cursor READ cursor WRITE setCursor NOTIFY cursorChanged)

	public:
		WindowPropertyBase(const QVariantMap *const params = nullptr, QObject *parent = nullptr);

		// Size property access

		constexpr int  width() const noexcept { return m_width; }
		Q_SLOT    void setWidth(const int width);
		Q_SIGNAL  void widthChanged(int newWidth);

		constexpr int  height() const noexcept { return m_height; }
		Q_SLOT    void setHeight(const int height);
		Q_SIGNAL  void heightChanged(int newHeight);

		constexpr QSize size() const noexcept { return {m_width, m_height}; }

		constexpr int  minimumWidth() const noexcept { return m_minimumWidth; }
		Q_SLOT    void setMinimumWidth(const int minimumWidth);
		Q_SIGNAL  void minimumWidthChanged(int newMinimumWidth);

		constexpr int  minimumHeight() const noexcept { return m_minimumHeight; }
		Q_SLOT    void setMinimumHeight(const int minimumHeight);
		Q_SIGNAL  void minimumHeightChanged(int newMinimumHeight);

		constexpr QSize minimumSize() const noexcept { return {m_minimumWidth, m_minimumHeight}; }

		constexpr int  maximumWidth() const noexcept { return m_maximumWidth; }
		Q_SLOT    void setMaximumWidth(const int maximumWidth);
		Q_SIGNAL  void maximumWidthChanged(int newMaximumWidth);

		constexpr int  maximumHeight() const noexcept { return m_maximumHeight; }
		Q_SLOT    void setMaximumHeight(const int maximumHeight);
		Q_SIGNAL  void maximumHeightChanged(int newMaximumHeight);

		constexpr QSize maximumSize() const noexcept { return {m_maximumWidth, m_maximumHeight}; }

		// Position property access
		
		constexpr int  x() const noexcept { return m_x; }
		Q_SLOT    void setX(const int x);
		Q_SIGNAL  void xChanged(int newX);

		constexpr int  y() const noexcept { return m_y; }
		Q_SLOT    void setY(const int y);
		Q_SIGNAL  void yChanged(int newY);

		constexpr QPoint position() const noexcept { return {m_x, m_y}; }

		// State property access

		constexpr bool isVisible() const noexcept { return m_visible; }
		Q_SLOT    void setVisible(const bool visible);
		Q_SIGNAL  void visibleChanged(bool newVisible);

		constexpr QWindow::Visibility  visibility() const noexcept { return m_visibility; }
		Q_SLOT    void                 setVisibility(const QWindow::Visibility visibility);
		Q_SIGNAL  void                 visibilityChanged(QWindow::Visibility newVisibility);

		constexpr bool isActive() const noexcept { return m_active; }

		constexpr bool isFullscreen() const noexcept { return m_visibility == QWindow::FullScreen; }
		Q_SLOT    void setFullscreen(const bool fullScreen);
		Q_SIGNAL  void fullscreenChanged(bool newFullScreen);

		// Appearance property access

		QString  title() const noexcept { return m_title; }
		Q_SLOT   void setTitle(const QString &title);
		Q_SIGNAL void titleChanged(const QString &newTitle);

		constexpr QColor color() const { return m_color; }
		Q_SLOT    void   setColor(const QColor &color);
		Q_SIGNAL  void   colorChanged(const QColor &newColor);

		constexpr double opacity() const noexcept { return m_opacity; }
		Q_SLOT    void   setOpacity(const double opacity);
		Q_SIGNAL  void   opacityChanged(double newOpacity);

		constexpr Qt::WindowFlags flags() const noexcept { return m_flags; }
		constexpr int             flagsInt() const noexcept { return m_flags; }
		Q_SLOT    void            setFlags(const Qt::WindowFlags flags);
		Q_SLOT    void            setFlagsInt(const int flags)
		{
			return setFlags(static_cast<Qt::WindowFlags>(flags));
		}
		Q_SIGNAL  void            flagsChanged(Qt::WindowFlags newFlags);
		Q_SIGNAL  void            flagsChangedInt(int newFlags);

		// Access to the bound window
		constexpr QQuickWindow* boundWindow() const noexcept { return m_boundWindow; }
		Q_SIGNAL  void          windowBound(QQuickWindow *const window);
		Q_SIGNAL  void          windowUnbound(QQuickWindow *const window);

	public slots:
		
		// Utility methods

		void setGeometry(int x, int y, int width, int height);
		void setPosition(int x, int y);
		void setSize(int width, int height);
		void centerOnScreen();
		void maximize();
		void minimize();
		void restore();

		// Bind to a QQuickWindow instance
		void bindToWindow(QQuickWindow *window);
		void unbindFromWindow();

	private:
		int m_width;                                // Size
		int m_height;                               // Size
		int m_minimumWidth;                         // Size
		int m_minimumHeight;                        // Size
		int m_maximumWidth;                         // Size
		int m_maximumHeight;                        // Size
		
		int m_x;                                    // Position
		int m_y;                                    // Position

		QWindow::Visibility m_visibility;           // State
		Qt::ScreenOrientation m_contentOrientation; // Content - Relocated for memory alignment.
		
		QString m_title;                            // Appearance
		QColor m_color;                             // Appearance
		double m_opacity;                           // Appearance
		Qt::WindowFlags m_flags;                    // Appearance - Relocated for memory alignment.
			
		int m_visible:1;                            // State
		int m_active:1;                             // State
		int m_clearBeforeRendering:1;               // Rendering
		int m_persistentOpenGLContext:1;            // Rendering
		int m_persistentSceneGraph:1;               // Rendering
		
		QScreen* m_screen;                          // Screen
		
		Qt::WindowModality m_modality;              // Modality
		
		Qt::CursorShape m_cursor;                   // Cursor
		
		QQuickWindow* m_boundWindow;                // Bound window
		
		// Store connections for cleanup
		std::optional<QMetaObject::Connection> m_connWidth;
		std::optional<QMetaObject::Connection> m_connHeight;
		std::optional<QMetaObject::Connection> m_connX;
		std::optional<QMetaObject::Connection> m_connY;
		std::optional<QMetaObject::Connection> m_connVisible;
		std::optional<QMetaObject::Connection> m_connFullScreen;
		std::optional<QMetaObject::Connection> m_connActive;
		std::optional<QMetaObject::Connection> m_connScreen;
		std::optional<QMetaObject::Connection> m_connColor;

		constexpr auto s() { return sizeof(*this); }
	};
}

#define TEMP sizeof(std::optional<QMetaObject::Connection>)