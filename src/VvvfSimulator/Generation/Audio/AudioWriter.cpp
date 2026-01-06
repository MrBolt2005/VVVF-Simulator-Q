#include "AudioWriter.hpp"

// Packages
#include <QList> // aka <QVector>

#define WARN_TEXT(x) "Cannot change the " x " attribute while the audio writer is open."

#define PROP_FAIL_TEXT(x) "The " x " attribute must be set before opening the audio writer."

#define LOCK_AND_SET_OPEN_CHECK(x) \
std::lock_guard locker(*(m_lock.data.load())); \
if (isOpen()) \
{ \
	m_err = { \
		QByteArrayLiteral(WARN_TEXT(x)), \
		{} \
	}; \
	qWarning() << tr(WARN_TEXT(x)); \
	return false; \
}

#define PROP_READY_FAIL(x) \
{ \
	m_err = { \
		QByteArrayLiteral(PROP_FAIL_TEXT(x)), \
		{} \
	}; \
	qWarning() << tr(PROP_FAIL_TEXT(x)); \
	return false; \
}

#define LOCK_AND_RETURN(x) \
std::lock_guard locker(*(m_lock.data.load())); \
return x;

namespace VvvfSimulator::Generation::Audio
{
	// TODO: Consider removing this implementation later
	AudioWriter::AudioWriter(QObject *parent)	
		: QObject(parent)
		// , m_flags(0)
		// , m_flags2(0)
	{
		m_channelLyt = av::ChannelLayout(m_channels);
	}

	AudioWriter::AudioWriter(
		const std::filesystem::path &fileName,
		int64_t bitRate,
		const std::pair<int64_t, int64_t> &bitRateRange,
		const av::Codec &codec,
		int channels,
		av::SampleFormat sampleFormat,
		int sampleRate,
		QObject *parent
	)
		: QObject(parent)
		, m_lock()
		, m_fileName(fileName)
		, m_bitRate(bitRate)
		, m_BRRange(bitRateRange)
		, m_codec(codec)
		, m_channels(channels)
		, m_channelLyt(channels)
		, m_smplFmt(sampleFormat)
		, m_smplRate(sampleRate)
	{
	}

	AudioWriter::~AudioWriter()
	{
		close();
	}

	int64_t AudioWriter::bitRate() const { LOCK_AND_RETURN(m_bitRate) }

	std::pair<int64_t, int64_t> AudioWriter::bitRateRange() const { LOCK_AND_RETURN(m_BRRange) }

	av::ChannelLayout AudioWriter::channelLayout() const { LOCK_AND_RETURN(m_channelLyt) }

	int AudioWriter::channels() const { LOCK_AND_RETURN(m_channels) }

	void AudioWriter::close()
	{
		if (!isOpen()) return;

		if (m_fmtCtx)
		{
			m_fmtCtx->flush();
			m_fmtCtx->writeTrailer();
		}
		m_isOpen = false;
		m_codecCtx.reset();
		m_fmtCtx.reset();
		m_strm.reset();
	}

	av::Codec AudioWriter::codec() const { LOCK_AND_RETURN(m_codec) }

	QString AudioWriter::errorString() const { LOCK_AND_RETURN(m_err.makeTrString()) }
	
	Util::String::TranslatableFmtString AudioWriter::errorStringRaw() const
	{ LOCK_AND_RETURN(m_err) }

	int AudioWriter::flags() const { LOCK_AND_RETURN(m_flags) }

	int AudioWriter::flags2() const { LOCK_AND_RETURN(m_flags2) }

	bool AudioWriter::open()
	{
		if (isOpen()) return true;

		decltype(Util::String::TranslatableFmtString::args) pendingProps;
		using arg = Util::String::FmtStringArg;

		// Check if we have everything set to open the writer
		if (m_fileName.empty())
			pendingProps << arg(Util::String::TranslatableFmtString
			{ QByteArrayLiteral(PROP_FAIL_TEXT("file name")), {} });
		/*
		if (!m_hasBR) pendingProps << Util::String::TranslatableFmtString
			{ QByteArrayLiteral(PROP_FAIL_TEXT("bit rate")), {} };
		if (!m_hasBRRange) pendingProps << Util::String::TranslatableFmtString
			{ QByteArrayLiteral(PROP_FAIL_TEXT("bit rate range")), {} };
		if (!m_hasChLyt) pendingProps << Util::String::TranslatableFmtString
			{ QByteArrayLiteral(PROP_FAIL_TEXT("channel layout")), {} };
		if (!m_hasChs) pendingProps << Util::String::TranslatableFmtString
			{ QByteArrayLiteral(PROP_FAIL_TEXT("channels number")), {} };
		if (!m_hasCodec) pendingProps << Util::String::TranslatableFmtString
			{ QByteArrayLiteral(PROP_FAIL_TEXT("codec")), {} };
		if (!m_hasSR) pendingProps << Util::String::TranslatableFmtString
			{ QByteArrayLiteral(PROP_FAIL_TEXT("sample rate")), {} };
		*/

		if (!pendingProps.isEmpty())
		{
			QByteArray args;
			for (qsizetype i = 0; i < pendingProps.size(); i++)
			{
				args += '%';
				args += QByteArray::number(i + 1);
				if (i < pendingProps.size() - 1) args += '\n';
			}

			m_err = {
				std::move(args),
				std::move(pendingProps)
			};
			
			qWarning() << m_err.makeTrString();
			return false;
		}

		if (m_strm) m_strm.reset(); // TODO: Check if this line is necessary

		m_fmtCtx.emplace();
		m_fmtCtx->openOutput(m_fileName.string(), m_opts);

		m_codecCtx.emplace();
		m_codecCtx->setBitRate(m_bitRate);
		m_codecCtx->setBitRateRange(m_BRRange);
		m_codecCtx->setCodec(m_codec);
		m_codecCtx->setFlags(m_flags);
		m_codecCtx->setFlags2(m_flags2);
		// Audio-specific codec ctx. initialization
		m_codecCtx->setChannels(m_channels);
		m_codecCtx->setChannelLayout(m_channelLyt);
		m_codecCtx->setSampleFormat(m_smplFmt);
		m_codecCtx->setSampleRate(m_smplRate);
		m_codecCtx->setStrict(m_strict);

		m_strm.emplace(std::move(m_fmtCtx->addStream(*m_codecCtx)));
		m_fmtCtx->writeHeader();

		m_isOpen = true;
		return true;
	}

	av::Dictionary AudioWriter::option() const { LOCK_AND_RETURN(m_opts) }

	bool AudioWriter::setBitRate(int64_t BR)
	{
		LOCK_AND_SET_OPEN_CHECK("bit rate")
		m_bitRate = BR;
		return true;
	}

	bool AudioWriter::setBitRateRange(const std::pair<int64_t, int64_t> &BRRange)
	{
		LOCK_AND_SET_OPEN_CHECK("bit rate range")
		m_BRRange = BRRange;
		return true;
	}

	bool AudioWriter::setChannelLayout(const av::ChannelLayout &chLyt, bool replaceChannelCount)
	{
		LOCK_AND_SET_OPEN_CHECK("channel layout")

		if (const int cnt = chLyt.channels(); cnt != m_channels)
		{
			if (!replaceChannelCount)
			{
				using arg = Util::String::FmtStringArg;
				m_err = {
					QByteArrayLiteral("The requested channel layout (%1: %2 "
						"channels) is invalid for the current channel count property "
						"value (%3 channels)."),
					{
						arg(QString(QString::fromStdString(chLyt.describe()))),
						arg(QString(QString::number(cnt))),
						arg(QString(QString::number(m_channels)))
					}
				};
				qWarning() << m_err.makeTrString();
				return false;
			}
			else m_channels = cnt;
		}

		m_channelLyt = chLyt;
		return true;
	}

	bool AudioWriter::setChannels(int chs)
	{
		LOCK_AND_SET_OPEN_CHECK("channel count")
		m_channels = chs;
		m_channelLyt = av::ChannelLayout(chs);
		return true;
	}
	
	bool AudioWriter::setCodec(const av::Codec &codec)
	{
		LOCK_AND_SET_OPEN_CHECK("codec")
		m_codec = codec;
		return true;
	}

	bool AudioWriter::setFileName(const std::filesystem::path &fileName)
	{
		LOCK_AND_SET_OPEN_CHECK("file name")
		m_fileName = fileName;
		return true;
	}

	bool AudioWriter::setFlags(const std::pair<int, int> &flags)
	{
		LOCK_AND_SET_OPEN_CHECK("flags")
		m_flags  = flags.first;
		m_flags2 = flags.second;
		return true;
	}

	bool AudioWriter::setFlags(int flags)
	{
		LOCK_AND_SET_OPEN_CHECK("flags")
		m_flags = flags;
		return true;
	}

	bool AudioWriter::setFlags2(int flags)
	{
		LOCK_AND_SET_OPEN_CHECK("flags")
		m_flags2 = flags;
		return true;
	}

	bool AudioWriter::setOptions(const av::Dictionary &opts)
	{
		LOCK_AND_SET_OPEN_CHECK("options")
		m_opts = opts;
		return true;
	}

	bool AudioWriter::setOptions(av::Dictionary &&opts)
	{
		LOCK_AND_SET_OPEN_CHECK("options")
		m_opts = std::move(opts);
		return true;
	}

	bool AudioWriter::setSampleFormat(av::SampleFormat fmt)
	{
		LOCK_AND_SET_OPEN_CHECK("sample format")
		m_smplFmt = fmt;
		return true;
	}

	bool AudioWriter::setSampleRate(int rate)
	{
		LOCK_AND_SET_OPEN_CHECK("sample rate")
		m_smplRate = rate;
		return true;
	}

	bool AudioWriter::setStrict(int strict)
	{
		LOCK_AND_SET_OPEN_CHECK("standard compliance strictness")
		m_strict = strict;
		return true;
	}

	int AudioWriter::strict() const { LOCK_AND_RETURN(m_strict) }

	void AudioWriter::writeSamples(const av::AudioSamples &samples, av::Packet *packet)
	{
		av::Packet pckt = m_codecCtx->encode(samples);
		m_fmtCtx->writePacket(pckt);
		
		emit samplesWritten(pckt.duration());
		if (packet) *packet = std::move(pckt);
	}
}