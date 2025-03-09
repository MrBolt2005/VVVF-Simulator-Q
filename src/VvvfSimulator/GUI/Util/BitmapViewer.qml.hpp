// Standard Library
#include <memory>
// Packages
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QImage>
#include <QObject>
#include <QPixmap>
#include <QSize>
#include <QString>

namespace VvvfSimulator::GUI::Util
{
    class BitmapViewer : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(ViewModel bindingData READ bindingData CONSTANT NOTIFY dataChanged)
        Q_PROPERTY(bool requireResize READ requireResize WRITE setRequireResize NOTIFY requireResizeChanged)

        class ViewModel : public QObject
        {
            Q_OBJECT
            Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap NOTIFY pixmapChanged)
            Q_PROPERTY(QGraphicsView& view READ view CONSTANT NOTIFY viewChanged)
            Q_PROPERTY(QGraphicsScene& scene READ scene CONSTANT NOTIFY sceneChanged)
            Q_PROPERTY(QGraphicsPixmapItem& item READ item CONSTANT NOTIFY itemChanged)

            QGraphicsView m_view;
            std::unique_ptr<QPixmap> m_pixmap;
            QGraphicsScene* m_scene;
            QGraphicsPixmapItem* m_item;

        public:
            //ViewModel();
            ViewModel(QObject* parent = nullptr);
            ViewModel(const QPixmap &pixmap, QObject* parent = nullptr);
            ~ViewModel() noexcept override = default;

            constexpr       QGraphicsView& view()       noexcept { return m_view; }
            constexpr const QGraphicsView& view() const noexcept { return m_view; }
            constexpr       QGraphicsScene& scene()       noexcept { return *m_scene; }
            constexpr const QGraphicsScene& scene() const noexcept { return *m_scene; }
            constexpr       QGraphicsPixmapItem& item()       noexcept { return *m_item; }
            constexpr const QGraphicsPixmapItem& item() const noexcept { return *m_item; }

        signals:
					void propertyChanged(const QString &propertyName);	
					void viewChanged(const QGraphicsView &newView);
					void pixmapChanged(const QPixmap *newPixmap);
					void sceneChanged(const QGraphicsScene *newScene);
					void itemChanged(const QGraphicsPixmapItem *newItem);

        public slots:
            QPixmap* pixmap() const noexcept { return m_pixmap.get(); }
            void setPixmap(const QPixmap &pixmap);
        };

        ViewModel m_bindingData;
				bool m_requireResize = true;

    public:
			//BitmapViewer();  
			BitmapViewer(QObject* parent = nullptr);
			~BitmapViewer() noexcept override;
		
			constexpr bool requireResize() const noexcept { return requireResize; }
			inline void setRequireResize(bool value) { 
				if (m_requireResize != value) {
					m_requireResize = value;
					emit requireResizeChanged(requireResize());
				}
      }

			void resize(const QSize &size) { m_bindingData.view().resize(size); }
			void setWindowTitle(const QString &title) { m_bindingData.view().setWindowTitle(title); }

    public slots:
        const ViewModel& bindingData() const noexcept { return m_bindingData; }
        void setPixmap(const QPixmap &pixmap, const QString* title = nullptr);
        void show() { m_bindingData.view().show(); }
        void close() { m_bindingData.view().close(); }

    signals:
    void dataChanged(const QString &propertyName);    
		void requireResizeChanged(bool newRequireResize);
    };
}