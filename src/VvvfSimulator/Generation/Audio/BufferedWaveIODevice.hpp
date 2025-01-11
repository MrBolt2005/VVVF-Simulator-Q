// Package Includes
#include <QBuffer>
#include <QByteArray>

namespace VvvfSimulator { namespace Generation { namespace Audio {
	// Allow injecting individual samples at a time for a Qt audio sink buffer.
	class BufferedWaveIODevice : public QIODevice
	{
		QByteArray m_buffer;
		qint64 m_pos = 0;
		qint64 m_maxSize = 80000;
	public:
		bool clearDataAfterRead = true;
		
		BufferedWaveIODevice(QObject *parent = nullptr, qint64 maxSize = 80000, bool __clearDataAfterRead = true)
			: QIODevice(parent), m_maxSize(maxSize) {}

		void addSample(const QByteArray& sample);
		bool open(OpenMode mode) override;
		qint64 readData(char *data, qint64 maxlen) override;
		qint64 writeData(const char *data, qint64 len) override;
		qint64 bytesAvailable() const override;
		qint64 size() const override;
		qint64 maxSize() const;
	};
}}}