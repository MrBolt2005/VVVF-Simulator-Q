// Internal Includes
#include "../BufferedWaveIODevice.hpp"
#include "../../GenerateCommon.hpp"
#include "../../../Vvvf/Struct.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"
// STL Includes
#include <functional>
#include <vector>
// Package Includes
#include <QAudioFormat>
#include <QAudioSink>
#include <QByteArray>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QObject>

namespace VvvfSimulator::Generation::Audio::VvvfSound::Audio
{
	class BufferedWaveFileWriter : public QObject
	{
		Q_OBJECT
		using BufferedWaveIODevice = VvvfSimulator::Generation::Audio::BufferedWaveIODevice;

		BufferedWaveIODevice *m_Buffer;
		QAudioSink *m_audioSink;
		QFile m_file;

	public:
		BufferedWaveFileWriter(const QDir& path, int samplingFrequency = 80000, QObject *parent = nullptr);
		//BufferedWaveFileWriter(const QDir& path, const QAudioFormat& audioFormat, QObject *parent = nullptr);
		~BufferedWaveFileWriter() override;
		void addSample(const QByteArray& sample);
		//void start();
		QByteArray floatToByteArray(float value);
	};
	static void downSample(int newSamplingRate, const QDir& inputPath, const QDir& outputPath, bool deleteOld);

	using GetSampleFunctional = std::function<std::vector<float>(
		NAMESPACE_VVVF::Struct::VvvfValues,
		NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData
	)>;

	static void exportWavFile(GenerationCommon::GenerationBasicParameter genParam, GetSampleFunctional getSample, int samplingFreq, bool useRaw, const QDir& Path);

//	public:
	static void exportWavLine(GenerationCommon::GenerationBasicParameter genParam, int samplingFreq, bool useRaw, const QDir& Path);
}
