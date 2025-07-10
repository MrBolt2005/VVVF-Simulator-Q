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