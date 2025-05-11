#include "WindowPropertyBase.hpp"

// Packages
#include <QGuiApplication>

namespace VvvfSimulator::GUI
{
	namespace
	{
	}
	
	WindowPropertyBase::WindowPropertyBase(const QVariantMap *const params, QObject *parent)
	{
		using namespace Qt::Literals::StringLiterals;
		
		#define FIND_VARIANT(prop, converter, dest, def) \
		it = params->constFind(u##prop##_s); \
		if (it != params->cend()) \
		{ \
			auto x =  it->converter(&ok); \
			dest = ok ? std::move(x) : def; \
		}	\
		else dest = def;

		#define FIND_VARIANT_NOCHECK(prop, converter, dest, def) \
		it = params->constFind(u##prop##_s); \
		dest = it != params->cend() ? it->converter() : def;

		#define FIND_VARIANT_VALUE(prop, dest, def) \
		it = params->constFind(u##prop##_s); \
		if (it != params->cend()) \
			dest = it->canConvert<decltype(dest)>() ? it->value<decltype(dest)>() : def;

		if (params)
		{
			bool ok;

			auto // it = params->constFind(...); ...
			FIND_VARIANT("width", toInt, m_width, 800)
			FIND_VARIANT("height", toInt, m_height, 600)
			FIND_VARIANT("minimumWidth", toInt, m_minimumWidth, 0)
			FIND_VARIANT("minimumHeight", toInt, m_minimumHeight, 0)
			FIND_VARIANT("maximumWidth", toInt, m_maximumWidth, 16777215)
			FIND_VARIANT("maximumHeight", toInt, m_maximumHeight, 16777215)
			FIND_VARIANT("x", toInt, m_x, 100)
			FIND_VARIANT("y", toInt, m_y, 100)
			FIND_VARIANT_NOCHECK("visible", toBool, m_visible, true)
			FIND_VARIANT_VALUE("visibility", m_visibility, QWindow::Windowed)
			FIND_VARIANT_NOCHECK("active", toBool, m_active, false)
			FIND_VARIANT_VALUE("title", m_title, QObject::tr("Title"))
			FIND_VARIANT_VALUE("color", m_color, Qt::white) // Should change to query (for) dark mode?
			FIND_VARIANT("opacity", toDouble, m_opacity, 1.0)
			FIND_VARIANT_VALUE("flags", m_flags, Qt::Window)

			// If no screen is set, use the primary screen
			FIND_VARIANT_VALUE("screen", m_screen, QGuiApplication::screens().size() > 0 ?
				QGuiApplication::primaryScreen() : nullptr)

			FIND_VARIANT_VALUE("contentOrientation", m_contentOrientation, Qt::PrimaryOrientation)
			FIND_VARIANT_VALUE("modality", m_modality, Qt::NonModal)
			FIND_VARIANT_NOCHECK("clearBeforeRendering", toBool, m_clearBeforeRendering, true)
			FIND_VARIANT_NOCHECK("persistentOpenGLContext", toBool, m_persistentOpenGLContext, false)
			FIND_VARIANT_NOCHECK("persistentSceneGraph", toBool, m_persistentSceneGraph, false)
			FIND_VARIANT_VALUE("cursor", m_cursor, Qt::ArrowCursor)
			FIND_VARIANT_VALUE("boundWindow", m_boundWindow, nullptr)
		}
			
		#undef FIND_VARIANT_NOCHECK
		#undef FIND_VARIANT_VALUE
		#undef FIND_VARIANT

		else
		// Initialize with defaults
		{
			m_width = 800;
			m_height = 600;
			m_minimumWidth = 0;
			m_minimumHeight = 0;
			m_maximumWidth = 16777215;
			m_maximumHeight = 16777215;
			m_x = 100;
			m_y = 100;
			m_visible = true;
			m_visibility = QWindow::Windowed;
			m_active = false;
			m_title = QObject::tr("Title");
			m_color = Qt::white;
			m_opacity = 1.0;
			m_flags = Qt::Window;
			m_screen = QGuiApplication::screens().size() > 0 ?
				QGuiApplication::primaryScreen() : nullptr;
			m_contentOrientation = Qt::PrimaryOrientation;
			m_modality = Qt::NonModal;
			m_clearBeforeRendering = true;
			m_persistentOpenGLContext = false;
			m_persistentSceneGraph = false;
			m_cursor = Qt::ArrowCursor;
			m_boundWindow = nullptr;
		}
	}

	#define SET_BODY_START(dest, src, boundSetter, optBoundCheck) \
	if (dest != src) \
	{ \
		dest = src; \
 \
		/* Apply to bound window if available */ \
		if (m_boundWindow && optBoundCheck) \
			m_boundWindow->boundSetter(src); \
 \

	#define SET_BODY_END }

	#define SET_BODY(dest, src, boundSetter, sig) \
	SET_BODY_START(dest, src, boundSetter, true) \
		emit sig(dest); \
	SET_BODY_END

	#define SET_BODY_BOUNDCHECK(dest, src, boundSetter, check, sig) \
	SET_BODY_START(dest, src, boundSetter, check) \
		emit sig(dest); \
	SET_BODY_END

	void WindowPropertyBase::setWidth(const int width)
	{
		SET_BODY_BOUNDCHECK(m_width, width, setWidth, m_visibility != QWindow::FullScreen, widthChanged)
	}

	void WindowPropertyBase::setHeight(const int height)
	{
		SET_BODY_BOUNDCHECK(m_height, height, setHeight, m_visibility != QWindow::FullScreen, heightChanged)
	}
	
	void WindowPropertyBase::setMinimumWidth(const int minimumWidth)
	{
		SET_BODY(m_minimumWidth, minimumWidth, setMinimumWidth, minimumWidthChanged)
	}

	void WindowPropertyBase::setMinimumHeight(const int minimumHeight)
	{
		SET_BODY(m_minimumHeight, minimumHeight, setMinimumHeight, minimumHeightChanged)
	}

	void WindowPropertyBase::setMaximumWidth(const int maximumWidth)
	{
		SET_BODY(m_maximumWidth, maximumWidth, setMaximumWidth, maximumWidthChanged)
	}

	void WindowPropertyBase::setMaximumHeight(const int maximumHeight)
	{
		SET_BODY(m_maximumHeight, maximumHeight, setMaximumHeight, maximumHeightChanged)
	}

	void WindowPropertyBase::setX(const int x)
	{
		SET_BODY_BOUNDCHECK(m_x, x, setX, m_visibility == QWindow::Windowed, xChanged)
	}

	void WindowPropertyBase::setY(const int y)
	{
		SET_BODY_BOUNDCHECK(m_y, y, setY, m_visibility == QWindow::Windowed, yChanged)
	}

	void WindowPropertyBase::setVisible(const bool visible)
	{
		SET_BODY(m_visible, visible, setVisible, visibleChanged)
	}

	void WindowPropertyBase::setVisibility(const QWindow::Visibility visibility)
	{
		const bool oldFullScreen = isFullscreen();
		SET_BODY_START(m_visibility, visibility, setVisibility, true)
			
			// Update fullScreen property based on visibility
			if (const bool fullScreen = isFullscreen(); fullScreen != oldFullScreen)
				emit fullscreenChanged(fullScreen);
			
			emit visibilityChanged(m_visibility);
		SET_BODY_END
	}

	void WindowPropertyBase::setFullscreen(const bool fullScreen)
	{
		if (isFullscreen() != fullScreen)
		{
			m_visibility = fullScreen ? QWindow::FullScreen : QWindow::Windowed;
			
			if (m_boundWindow)
				m_boundWindow->setVisibility(m_visibility);
			
			emit visibilityChanged(m_visibility);
			emit fullscreenChanged(isFullscreen());
		}
	}

	void WindowPropertyBase::setTitle(const QString &title)
	{
		SET_BODY(m_title, title, setTitle, titleChanged)
	}

	void WindowPropertyBase::setColor(const QColor &color)
	{
		SET_BODY(m_color, color, setColor, colorChanged)
	}

	void WindowPropertyBase::setOpacity(const double opacity)
	{
		if (!qFuzzyCompare(m_opacity, opacity))
		{
			m_opacity = opacity;

			if (m_boundWindow)
				m_boundWindow->setOpacity(static_cast<qreal>(opacity));
        
			emit opacityChanged(m_opacity);
    }
	}

	void WindowPropertyBase::setFlags(const Qt::WindowFlags flags)
	{
		if (m_flags != flags)
		{
			m_flags = flags;
			
			if (m_boundWindow && true)
				m_boundWindow->setFlags(flags);
				
			emit flagsChanged(m_flags);
			emit flagsChangedInt(m_flags);
		}
	}

	#undef SET_BODY_BOUNDCHECK
	#undef SET_BODY
	#undef SET_BODY_END
	#undef SET_BODY_START

	void WindowPropertyBase::bindToWindow(QQuickWindow *window)
	{
		// Clean up previous binding if any
		unbindFromWindow();
			
		if (window)
		{
			m_boundWindow = window;
			
			// Initial synchronization from our properties to window
			m_boundWindow->setWidth(m_width);
			m_boundWindow->setHeight(m_height);
			m_boundWindow->setMinimumWidth(m_minimumWidth);
			m_boundWindow->setMinimumHeight(m_minimumHeight);
			m_boundWindow->setMaximumWidth(m_maximumWidth);
			m_boundWindow->setMaximumHeight(m_maximumHeight);
			m_boundWindow->setX(m_x);
			m_boundWindow->setY(m_y);
			m_boundWindow->setTitle(m_title);
			m_boundWindow->setColor(m_color);
			m_boundWindow->setOpacity(m_opacity);
			m_boundWindow->setFlags(m_flags);
			m_boundWindow->setScreen(m_screen);
			m_boundWindow->setContentOrientation(m_contentOrientation);
			m_boundWindow->setModality(m_modality);
			m_boundWindow->setClearBeforeRendering(m_clearBeforeRendering);
			m_boundWindow->setPersistentOpenGLContext(m_persistentOpenGLContext);
			m_boundWindow->setPersistentSceneGraph(m_persistentSceneGraph);
			m_boundWindow->setCursor(m_cursor);
        
			// Set visibility and window state last
			m_boundWindow->setVisibility(static_cast<QWindow::Visibility>(m_visibility));
        
			m_boundWindow->setVisible(m_visible);
        
			// Set up connections
			m_connWidth = connect(m_boundWindow, &QQuickWindow::widthChanged, this, [this]()
			{
				if (m_boundWindow && m_visibility != QWindow::FullScreen)
				{
					const int newWidth = m_boundWindow->width();
					if (m_width != newWidth)
					{
						m_width = newWidth;
						emit widthChanged(m_width);
					}
				}
			});

			// Note: Many QQuickWindow properties don't emit change signals
			// So we can't connect to them (opacity, cursor, etc.)
			emit windowBound(m_boundWindow);
		}
	}

	void WindowPropertyBase::unbindFromWindow()
	{
		if (m_boundWindow)
		{
			// Disconnect all stored connections
			if (m_connWidth)
			{
				disconnect(*m_connWidth);
				m_connWidth.reset();	
			}
			emit windowUnbound(m_boundWindow);
			m_boundWindow = nullptr;
		}
	}
}

#if       0
#include "moc_WindowPropertyBase.cpp"
#endif // 0
