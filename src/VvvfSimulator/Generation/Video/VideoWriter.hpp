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
#include <QSize>
// Internal
#include "../../Util/String.hpp"
#include "../../Util/Thread.hpp"

namespace VvvfSimulator::Generation::Video
{
	class VideoWriter : public QObject
	{
		Q_OBJECT

		explicit VideoWriter(QObject *parent = nullptr);

	public:
		VideoWriter(
			const std::filesystem::path &fileName,
			int64_t bitRate,
			const std::pair<int64_t, int64_t> &bitRateRange,
			const av::Codec &codec,
			av::PixelFormat pixelFormat,
			const QSize &size,
			const av::Rational &timeBase,
			QObject *parent = nullptr
		);
		~VideoWriter() override;

		// Getters
		int64_t                             bitRate()        const;
		std::pair<int64_t, int64_t>         bitRateRange()   const;
		av::Codec                           codec()          const;
		QString                             errorString()    const;
		Util::String::TranslatableFmtString errorStringRaw() const;
		std::filesystem::path               fileName()       const;
		int                                 flags()          const;
		int                                 flags2()         const;
		bool																isOpen()         const;
		av::Dictionary                      option()         const;
		av::PixelFormat                     pixelFormat()    const;
		QSize                               size()           const;
		av::Rational                        timeBase()       const;
		int																  strict()         const;

	public slots:
		// Setters
		bool setBitRate(int64_t bitRate);
		bool setBitRateRange(const std::pair<int64_t, int64_t> &bitRateRange);
		bool setCodec(const av::Codec &codec);
		bool setFileName(const std::filesystem::path &fileName);
		bool setFlags(const std::pair<int, int> &flags);
		bool setFlags(int flags);
		bool setFlags2(int flags);
		bool setOptions(const av::Dictionary &opts);
		bool setOptions(av::Dictionary &&opts);
		bool setPixelFormat(av::PixelFormat pixelFormat);
		bool setSize(const QSize &size);
		bool setTimeBase(const av::Rational &timeBase);
		bool setStrict(int strict);

		void close();
		bool open();
		void write(const QImage &image, av::Packet *packet = nullptr);

	private:
		Util::Thread::MutexAPtrWrapper m_lock;

		// Category-independent context props
		int64_t m_bitRate;
		std::pair<int64_t, int64_t> m_BRRange;
		av::Codec m_codec;
		std::filesystem::path m_fileName;
		int m_flags, m_flags2;
		av::Dictionary m_opts;
		av::Rational m_tb;
		int m_strict;

		// Video-specific context props
		av::PixelFormat m_pixFmt;
		QSize m_size;
		int64_t m_pts;

		// Optional audio context props (TODO~?)

		std::optional<av::FormatContext> m_fmtCtx;
		std::optional<av::VideoEncoderContext> m_codecCtx;
		std::optional<av::Stream> m_strm;
		
		Util::String::TranslatableFmtString m_err;
		bool m_isOpen;
	};

	#undef TEMP
	#define TEMP sizeof(VideoWriter)
}