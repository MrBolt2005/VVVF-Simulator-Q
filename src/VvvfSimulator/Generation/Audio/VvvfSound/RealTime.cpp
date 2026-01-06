#include "RealTime.hpp"

// Packages
#include <QAudioSink>
#include <QByteArray>
#include <QFuture>
#include <QScopedPointer>
#include <QtConcurrent/QtConcurrent>
// Internal
#include "../BufferedWaveIODevice.hpp"
#include "../../../Outcome.hpp"
#include "../../../Vvvf/Calculate.hpp"
#include "../../../Vvvf/Struct.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfWave.hpp"

namespace VvvfSimulator::Properties::Settings::Default
{
	extern int RealtimeVvvfCalculateDivision;
	extern int RealtimeVvvfSamplingFrequency;
	extern int RealTime_VVVF_BuffSize;
}

namespace VvvfSimulator::Generation::Audio::VvvfSound::RealTime
{
	namespace
	{
		inline Outcome::Result<void, std::variant<QSerialPort::SerialPortError, std::exception_ptr>> generate
		(
			BufferedWaveIODevice &provider,
			const Yaml::VvvfSound::YamlVvvfSoundData &soundData,
			Vvvf::Struct::VvvfValues &control,
			GenerateRealTimeCommon::RealTimeParameter &param,
			QSerialPort &serial
		)
		{
			if (!serial.open(QIODevice::ReadWrite))
				return serial.error();

			int endResult;
			while (true)
			{
				const int &calcCount = Properties::Settings::Default::RealtimeVvvfCalculateDivision;
				const double Dt = 1.0 / Properties::Settings::Default::RealtimeVvvfSamplingFrequency;

				endResult = GenerateRealTimeCommon::realTimeFrequencyControl(control, param, calcCount * Dt);
				if (endResult != -1) break;

				QByteArray data;
				data.reserve(calcCount);

				for (int i = 0; i < calcCount; i++)
				{
					control.sinTime += Dt;
					control.sawTime += Dt;
					control.generationCurrentTime += Dt;

					Vvvf::Struct::PwmCalculateValues calculated_Values = Yaml::VvvfSound::YamlVvvfWave::calculateYaml(control, soundData);
					Vvvf::Struct::WaveValues value = Vvvf::Calculate::calculatePhases(control, calculated_Values, 0.0);
					data.push_back(value.U << 4 | value.V << 2 | value.W);

					const float soundByte = (value.U - value.V) * 0.35;

					provider.addSample(QByteArray
					(
						reinterpret_cast<const char *const>(&soundByte),
						sizeof(soundByte) / sizeof(char)
					));
				}

				static QFuture<qint64> lastWrite;
				if (lastWrite.isRunning())
				{
					// Wait for previous write to complete
					lastWrite.waitForFinished();
					if (lastWrite.result() < 0) break;
				}

				lastWrite = QtConcurrent::run([&data, &serial]()
				{
					return serial.write(data);
				});

				while (provider.bufferedBytes() + calcCount > Properties::Settings::Default::RealTime_VVVF_BuffSize);
			}

			constexpr char trailer = 0xFF;
			if (serial.write(&trailer, sizeof(trailer) / sizeof(char)) < 0) return serial.error();
			try
			{
				serial.close();
			}
			catch (...)
			{
				return std::current_exception();
			}

			return Outcome::success();
		}
	}
	
	Outcome::Result<void, std::variant<QSerialPort::SerialPortError, std::exception_ptr>> calculate
	(
		const Yaml::VvvfSound::YamlVvvfSoundData &sound,
		GenerateRealTimeCommon::RealTimeParameter &param,
		QSerialPort &serial
	)
	{
		if (param.audioDevice.isNull()) return -1;

		param.quit = false;
		param.vvvfSoundData = sound;

		param.control = Vvvf::Struct::VvvfValues();

		QScopedPointer<BufferedWaveIODevice> provider(new BufferedWaveIODevice());
		// provider.setDiscardOnBufferOverflow(true);
		QAudioSink aSink(*(param.audioDevice), QAudioFormat::Float);

		aSink.start(provider.get());

		Outcome::Result<void, std::variant<QSerialPort::SerialPortError, std::exception_ptr>> stat;
		try
		{
			stat = generate(*provider, sound, control, param, serial);
		}
		catch (...)
		{
			stat = std::current_exception();
		}
		finally
		{
			aSink.stop();
		}

		return stat;
	}
}