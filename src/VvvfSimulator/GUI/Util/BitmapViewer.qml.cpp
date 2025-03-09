#include "BitmapViewer.qml.hpp"

namespace VvvfSimulator::GUI::Util
{
	/*
	BitmapViewer::ViewModel::ViewModel() :
		m_view(),
		m_pixmap(nullptr),
		m_scene(new QGraphicsScene(this)),
		m_item(nullptr)
	{}
		*/

	BitmapViewer::ViewModel::ViewModel(QObject *parent) :
		QObject(parent),
		m_view(),
		m_pixmap(nullptr),
		m_scene(new QGraphicsScene(this)),
		m_item(nullptr)
	{
		QObject::connect(this, &ViewModel::pixmapChanged, this, [&]() { emit propertyChanged(QStringLiteral("pixmap")); });
		QObject::connect(this, &ViewModel::sceneChanged, this, [&]() { emit propertyChanged(QStringLiteral("scene")); });
		QObject::connect(this, &ViewModel::viewChanged, this, [&]() { emit propertyChanged(QStringLiteral("view")); });
		QObject::connect(this, &ViewModel::itemChanged, this, [&]() { emit propertyChanged(QStringLiteral("item")); });
	}

	BitmapViewer::ViewModel::ViewModel(const QPixmap &pixmap, QObject* parent = nullptr) :
		QObject(parent),
		m_view(),
		m_pixmap(std::make_unique<QPixmap>(pixmap)),
		m_scene(new QGraphicsScene(this)),
		m_item(m_scene->addPixmap(*m_pixmap))
	{
		QObject::connect(this, &ViewModel::pixmapChanged, this, [&]() { emit propertyChanged(QStringLiteral("pixmap")); });
		QObject::connect(this, &ViewModel::sceneChanged, this, [&]() { emit propertyChanged(QStringLiteral("scene")); });
		QObject::connect(this, &ViewModel::viewChanged, this, [&]() { emit propertyChanged(QStringLiteral("view")); });
		QObject::connect(this, &ViewModel::itemChanged, this, [&]() { emit propertyChanged(QStringLiteral("item")); });
	}

	BitmapViewer::ViewModel::~ViewModel() noexcept = default;

	void BitmapViewer::ViewModel::setPixmap(const QPixmap &pixmap)
	{
		if (m_pixmap) *m_pixmap = pixmap;
		else m_pixmap.reset(new QPixmap(pixmap));
		Q_ASSERT(m_pixmap);
		if (m_item)	m_item->setPixmap(*m_pixmap);
		else
		{
			Q_ASSERT(m_scene);
			m_item = m_scene->addPixmap(*m_pixmap);
		}
		//emit propertyChanged(QStringLiteral("pixmap,item,scene"));
		emit pixmapChanged(m_pixmap.get());
		emit itemChanged(m_item);
		emit sceneChanged(m_scene);
	}

	void BitmapViewer::setPixmap(const QPixmap &pixmap, const QString *title)
	{
		if (requireResize())
		{
			resize(pixmap.size());
			setWindowTitle(title ? *title : QObject::tr("Bitmap Viewer"));
			setRequireResize(false);
		}

		setPixmap(pixmap);
	}

	/*
	BitmapViewer::BitmapViewer()
	{
		QObject::connect(&bindingData(), &BitmapViewer::ViewModel::propertyChanged, this, &BitmapViewer::dataChanged);
	}
		*/

	BitmapViewer::BitmapViewer(QObject* parent) :
		QObject(parent)
	{
		QObject::connect(&m_bindingData, &BitmapViewer::ViewModel::propertyChanged, this, &BitmapViewer::dataChanged);
	}
	
	BitmapViewer::~BitmapViewer() noexcept = default;
}
