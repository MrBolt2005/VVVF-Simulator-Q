// Internal Includes
#include <BufferedWaveIODevice.hpp>
#include <Struct.hpp>
#include <YamlVvvfAnalyze.hpp>
// STL Includes
#include <functional>
#include <memory>
#include <vector>
// Package Includes
#include <QAudioFormat>
#include <QAudioSink>
#include <QByteArray>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QObject>

namespace VvvfSimulator::Generation::Audio::VvvfSound
{
	class BufferedWaveFileWriter : public QObject
	{
		Q_OBJECT
		using VvvfSimulator::Generation::Audio::BufferedWaveIODevice;

		std::unique_ptr<BufferedWaveIODevice> m_Buffer;
		std::unique_ptr<QAudioSink> m_audioSink;
		QFile m_file;

	public:
		BufferedWaveFileWriter(const QDir& path, int samplingFrequency = 80000, QObject *parent = nullptr);
		~BufferedWaveFileWriter() override;
		void addSample(const QByteArray& sample);
	//	void start();
		QByteArray floatToByteArray(float value);
	};
	static void downSample(int newSamplingRate, const QString& inputPath, const QDir& outputPath, bool deleteOld);

	using GetSampleFunctional = std::function<std::vector<float>(VvvfValues, YamlVvvfSoundData)>;

	static void exportWavFile(GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, const QDir& Path);

//	public:
	static void exportWavLine(GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const QDir& Path);
}
