#pragma once
#ifndef GENERATION__AUDIO__AUDIOWRITER_HPP
#define GENERATION__AUDIO__AUDIOWRITER_HPP

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Library/AudioWriter.hpp
 * v1.10.0.1
 */

// Standard Library
#include <memory>
// Packages
#include <QPair>
#include <QSpan>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
// Internal
#include "../IWriter.hpp"

namespace VvvfSimulator::Generation::Audio {
class AudioWriter : public IWriter {
  public:
    typedef QPair<int64_t, int64_t> BitRateRangeType;

    BitRateRangeType bitRateRange() const;

  public slots:
    void open(const QUrl *url = nullptr,
              boost::logic::tribool *ok = nullptr) override;
    void write(QSpan<const uint8_t> in, bool *ok = nullptr);

  protected:
    void closeLockless(bool failOnError, boost::logic::tribool *ok) override;

  private:
    friend class AudioWriterPrivate;

    typedef decltype(avformat_free_context) *FmtCtxFreeT;
    typedef std::unique_ptr<AVFormatContext, FmtCtxFreeT> FmtCtxPtrT;

    // Common parameters
    int64_t m_bitRate;
    int m_brTolerance;
    AVCodec *m_codec;
    int m_flags, m_flags2;
    AVDictionary *m_opts;
    int m_strict;

    // Audio-specific parameters
    int m_channels;
    AVChannelLayout m_channelLyt;
    AVSampleFormat m_smplFmt;
    int m_smplRate;

    // Implementation details
    FmtCtxPtrT m_fmtCtx;
    AVCodecContext *m_codecCtx;
    AVStream *m_strm; // Owned by m_fmtCtx
    int64_t m_pts;

    // Enforce construction with the public constructor **only**
    AudioWriter() = delete;
    Q_DISABLE_COPY(AudioWriter)

    constexpr auto sizeOf() const noexcept {
        return sizeof(*this);
    }
};
} // namespace VvvfSimulator::Generation::Audio

#endif // GENERATION__AUDIO__AUDIOWRITER_HPP