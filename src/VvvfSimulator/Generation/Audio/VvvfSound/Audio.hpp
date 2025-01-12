// Internal Includes
#include <BufferedWaveIODevice.hpp>
#include <Struct.hpp>
#include <YamlVvvfAnalyze.hpp>
// STL Includes
#include <filesystem>
#include <functional>
#include <memory>
#include <vector>
// Package Includes
#include <QAudioFormat>
#include <QAudioSink>
#include <QByteArray>
#include <QBuffer>
#include <QFile>
#include <QObject>
#include <QString>

namespace VvvfSimulator::Generation::Audio::VvvfSound
{
	class Audio
	{
		class BufferedWaveFileWriter : public QObject
		{
			Q_OBJECT
			using VvvfSimulator::Generation::Audio::BufferedWaveIODevice;

			std::unique_ptr<BufferedWaveIODevice> Buffer;
			std::unique_ptr<QAudioSink> m_audioSink;
			QFile m_file;

		public:
			BufferedWaveFileWriter(const QString& path, int samplingFrequency, QObject *parent = nullptr);
			~BufferedWaveFileWriter() override;
			void addSample(const QByteArray& sample);
		//	void start();
			QByteArray floatToByteArray(float value);
		};
		static void downSample(int newSamplingRate, const QString& inputPath, const QString& outputPath, bool deleteOld);

		using GetSampleFunctional = std::function<std::vector<float>(VvvfValues, YamlVvvfSoundData)>;

		static void exportWavFile(GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, std::filesystem::path Path);

	public:
		static void exportWavLine(GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const QString& Path);
	};
}
