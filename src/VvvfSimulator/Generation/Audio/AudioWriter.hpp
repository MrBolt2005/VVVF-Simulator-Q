#pragma once

// Standard Library
#include <filesystem>
#include <optional>
#include <utility>
// Packages
#include <avcpp/codec.h>
#include <avcpp/codeccontext.h>
#include <avcpp/format.h>
#include <avcpp/formatcontext.h>
#include <avcpp/frame.h>
#include <avcpp/packet.h>
#include <QObject>
// Internal
#include "../../Util/String.hpp"
#include "../../Util/Thread.hpp"

namespace VvvfSimulator::Generation::Audio
{
	class AudioWriter : public QObject
	{
		Q_OBJECT
		// using AudioWriterCodecContextType = av::AudioCodecContext<av::AudioEncoderContext, av::Direction::Encoding>;

		explicit AudioWriter(QObject *parent = nullptr);

	public:
		// Constructor size was getting out of hand, let's
		// just use setters after making the object anyway!
		/*
		Stuff that must be defined for making an audio writer:
			Filename
			Bit rate
			BR range
			Codec
			Channels
			Sample Format
			Sample Rate
		*/
		AudioWriter(
			const std::filesystem::path &fileName,
			int64_t bitRate,
			const std::pair<int64_t, int64_t> &bitRateRange,
			const av::Codec &codec,
			int channels,
			av::SampleFormat sampleFormat,
			int sampleRate,
			QObject *parent = nullptr
		);
		~AudioWriter() override;
	
		// Getters
		int64_t                             bitRate()        const;
		std::pair<int64_t, int64_t>         bitRateRange()   const;
		av::ChannelLayout                   channelLayout()  const;
		int                                 channels()       const;
		av::Codec                           codec()          const;
		QString                             errorString()    const;
		Util::String::TranslatableFmtString errorStringRaw() const;
		std::filesystem::path               fileName()       const;
		int                                 flags()          const;
		int                                 flags2()         const;
		bool                                isOpen()         const;
		av::Dictionary                      option()         const;
		av::SampleFormat                    sampleFormat()   const;
		int                                 sampleRate()     const;
		int                                 strict()         const;

	public slots:
		// Setters
		bool setBitRate(int64_t BR);
		bool setBitRateRange(const std::pair<int64_t, int64_t> &BRRange);
		bool setChannelLayout(const av::ChannelLayout &chLyt, bool replaceChannelCount = false);
		bool setChannels(int chs);
		bool setCodec(const av::Codec &codec);
		bool setFileName(const std::filesystem::path &fileName);
		bool setFlags(const std::pair<int, int> &flags);
		bool setFlags(int flags);
		bool setFlags2(int flags);
		bool setOptions(const av::Dictionary &opts);
		bool setOptions(av::Dictionary &&opts);
		bool setSampleFormat(av::SampleFormat fmt);
		bool setSampleRate(int rate);
		bool setStrict(int strict);
	
		void close();
		bool open();
		void writeSamples(const av::AudioSamples &samples, av::Packet *packet = nullptr);

	signals:
		void samplesWritten(int duration);

	private:
		Util::Thread::MutexAPtrWrapper m_lock;

		// Category-independent context props
		int64_t m_bitRate;
		std::pair<int64_t, int64_t> m_BRRange;
		av::Codec m_codec;
		std::filesystem::path m_fileName;
		int m_flags, m_flags2;
		av::Dictionary m_opts;
		int m_strict;
		// av::Rational m_tb;

		// Audio-specific context props
		int m_channels;
		av::ChannelLayout m_channelLyt;
		av::SampleFormat m_smplFmt;
		int m_smplRate;

		std::optional<av::FormatContext> m_fmtCtx;
		std::optional<av::AudioEncoderContext> m_codecCtx;
		std::optional<av::Stream> m_strm;
		
		Util::String::TranslatableFmtString m_err;
		bool m_isOpen;
	};

	#undef TEMP
	#define TEMP sizeof(AudioWriter)
}