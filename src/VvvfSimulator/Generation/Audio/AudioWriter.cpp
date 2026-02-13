#include "AudioWriter.hpp"

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Library/AudioWriter.cpp
 * v1.10.0.1
 */

// Standard Library
#include <exception>
// Packages
#include <QByteArray>
#include <QUrl>

#define DEFAULT_LOCK_PROLOGUE QMutexLocker locker(&m_lock);
#define LOCK_AND_RETURN(x) DEFAULT_LOCK_PROLOGUE return x;
#define SET_IF_PTR(ptr, val)                                                   \
    if (ptr)                                                                   \
        *ptr = val;
#define GET_AND_CAST_FUNC(symbol)                                              \
    reinterpret_cast<decltype(##symbol) *>(                                    \
        awp.getCachedFuncPtr(QByteArrayLiteral(#symbol)))

namespace VvvfSimulator::Generation::Audio {
namespace {
constexpr auto UNKNOWN_ERR_CSTR = "Unknown error.";
}
class AudioWriterPrivate {
  public:
    AudioWriter *aw;
    AudioWriterPrivate(AudioWriter *aWriter)
        : aw(aWriter) {}

    QFunctionPointer getCachedFuncPtr(const QByteArray &symbol) {
        auto it = aw->m_localFuncCache.find(symbol);
        Q_ASSERT(it != aw->m_localFuncCache.end());
        return it.value();
    }
};

void AudioWriter::open(const QUrl *url, boost::logic::tribool *ok) {
    DEFAULT_LOCK_PROLOGUE
    using namespace Qt::Literals::StringLiterals;

    if (m_isOpen) {
        SET_IF_PTR(ok, boost::logic::indeterminate)
        return;
    }
    AudioWriterPrivate awp(this);

    AVDictionary *optsCopy = nullptr;

    // Fetch cached functions
    auto _av_dict_free = GET_AND_CAST_FUNC(av_dict_free);
    auto _avcodec_free_context = GET_AND_CAST_FUNC(avcodec_free_context);
    auto _avformat_free_context = GET_AND_CAST_FUNC(avformat_free_context);

    // Util::String::TranslatableFmtString error;
    try {
        const QUrl *currUrl = url ? url : &m_url;

        auto _avformat_alloc_context =
            GET_AND_CAST_FUNC(avformat_alloc_context);
        /*
         * avformat_network_init is going to be deprecated in the foreseeable
         * future, so we should be safe skipping it~
         */
        auto _av_strdup = GET_AND_CAST_FUNC(av_strdup);
        auto _avcodec_alloc_context3 =
            GET_AND_CAST_FUNC(avcodec_alloc_context3);
        auto _avformat_new_stream = GET_AND_CAST_FUNC(avformat_new_stream);
        auto _av_dict_copy = GET_AND_CAST_FUNC(av_dict_copy);
        auto _av_dict_set = GET_AND_CAST_FUNC(av_dict_set);
        auto _av_strerror = GET_AND_CAST_FUNC(av_strerror);

        QByteArray urlCString;

        // Initialize AVFormatContext
        m_fmtCtx = _avformat_alloc_context();
        if (!m_fmtCtx) {
            m_err.reset();
            m_err.sourceText =
                "Memory error: failed to initialize the audio writer's format context."_ba;
            goto fail; // fail2?
        }

        urlCString = currUrl->isLocalFile()
                         ? currUrl->toLocalFile().toLocal8Bit()
                         : currUrl->url().toUtf8();
        m_fmtCtx->url = _av_strdup(urlCString);
        if (!m_fmtCtx->url) {
            m_err.reset();
            m_err.sourceText =
                "Memory error: failed to set URL in the audio writer's format context."_ba;
            goto fail;
        }

        // Initialize AVCodecContext
        m_codecCtx = _avcodec_alloc_context3(m_codec);
        if (!m_codecCtx) {
            m_err.reset();
            m_err.sourceText =
                "Memory error: failed to initialize the audio writer's codec context."_ba;
            goto fail;
        }
        m_codecCtx->bit_rate = m_brRange;
        m_codecCtx->bit_rate_tolerance = m_brTolerance;
        // m_codecCtx->codec = m_codec;
        m_codecCtx->flags = m_flags;
        m_codecCtx->flags2 = m_flags2;
        // time base = 1 / sample rate
        m_codecCtx->time_base.num = 1;
        m_codecCtx->time_base.den = m_smplRate;

        // Audio-specific codec ctx. initialization
        // TODO: Correct this following line
        m_codecCtx->ch_layout = *m_channelLyt;
        m_codecCtx->sample_fmt = m_smplFmt;
        m_codecCtx->sample_rate = m_smplRate;
        m_codecCtx->strict_std_compliance = m_strict;

        // Initialize AVStream
        m_strm = _avformat_new_stream(m_fmtCtx, m_codecCtx->codec);
        if (!m_strm)
            ;

        // Copy the options dictionary
        int eCode;
        if (m_opts) {
            eCode = _av_dict_copy(&optsCopy, m_opts, 0);
        } else {
            eCode = _av_dict_set(nullptr, nullptr, nullptr, 0);
        }
        if (eCode < 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to set up the audio writer's options: %1"_ba;
            m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                              .value_or(qTr(UNKNOWN_ERR_CSTR));
            goto fail;
        }

        eCode = avformat_init_output(m_fmtCtx, &optsCopy);
        if (eCode < 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to initialize the audio writer's output: %1"_ba;
            m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                              .value_or(qTr(UNKNOWN_ERR_CSTR));
            goto fail;
        } else if (eCode == AVSTREAM_INIT_IN_WRITE_HEADER) {
            qDebug().nospace()
                << QStringView(u"avformat_init_output() -> "
                               u"AVSTREAM_INIT_IN_WRITE_HEADER (")
                << eCode << u')';
        } else if (eCode == AVSTREAM_INIT_IN_INIT_OUTPUT) {
            qDebug().nospace() << QStringView(u"avformat_init_output() -> "
                                              u"AVSTREAM_INIT_IN_INIT_OUTPUT (")
                               << eCode << u')';
        }
        eCode = avformat_write_header(m_fmtCtx, nullptr);
        if (eCode < 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to write the file header on the audio writer's output: %1"_ba;
            m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                              .value_or(qTr(UNKNOWN_ERR_CSTR));
            goto fail;
        } else if (eCode == AVSTREAM_INIT_IN_WRITE_HEADER) {
            qDebug().nospace()
                << QStringView(u"avformat_write_header() -> "
                               u"AVSTREAM_INIT_IN_WRITE_HEADER (")
                << eCode << u')';
        } else if (eCode == AVSTREAM_INIT_IN_INIT_OUTPUT) {
            qDebug().nospace() << QStringView(u"avformat_write_header() -> "
                                              u"AVSTREAM_INIT_IN_INIT_OUTPUT (")
                               << eCode << u')';
        }

        m_isErrorLast = false;
        m_isOpen = true;
        if (currUrl != &m_url)
            m_url = *currUrl;
        SET_IF_PTR(ok, true);
        return;

    fail:
        m_isErrorLast = true;
        // fail2:
        SET_IF_PTR(ok, false);
    } catch (const std::exception &ex) {
        if (optsCopy) {
            _av_dict_free(&optsCopy);
        }
        if (m_codecCtx) {
            _avcodec_free_context(&m_codecCtx);
            m_codecCtx = nullptr;
        }
        if (m_fmtCtx) {
            _avformat_free_context(m_fmtCtx);
            m_fmtCtx = nullptr;
        }

        m_err.reset();
        m_err.sourceText = "Exception thrown: %1"_ba;
        m_isErrorLast = true;
        m_err.args << QString(ex.what());
        SET_IF_PTR(ok, false);
    }
}

void AudioWriter::write(const std::span<const uint8_t> &in, bool *ok) {
    DEFAULT_LOCK_PROLOGUE
    using namespace Qt::Literals::StringLiterals;
    AudioWriterPrivate awp(this);

    int eCode, depth, isPlanar;
    size_t multiplier, smplCount, smplPerChCount;
    AVFrame *frame;
    AVPacket *packet = nullptr;

    auto _av_get_bytes_per_sample = GET_AND_CAST_FUNC(av_get_bytes_per_sample);
    auto _av_sample_fmt_is_planar = GET_AND_CAST_FUNC(av_sample_fmt_is_planar);
    auto _av_frame_alloc = GET_AND_CAST_FUNC(av_frame_alloc);
    auto _av_frame_get_buffer = GET_AND_CAST_FUNC(av_frame_get_buffer);
    auto _av_frame_free = GET_AND_CAST_FUNC(av_frame_free);
    auto _av_strerror = GET_AND_CAST_FUNC(av_strerror);
    auto _av_frame_make_writable = GET_AND_CAST_FUNC(av_frame_make_writable);
    auto _avcodec_send_frame = GET_AND_CAST_FUNC(avcodec_send_frame);
    auto _avcodec_receive_packet = GET_AND_CAST_FUNC(avcodec_receive_packet);
    auto _av_packet_rescale_ts = GET_AND_CAST_FUNC(av_packet_rescale_ts);
    auto _av_interleaved_write_frame =
        GET_AND_CAST_FUNC(av_interleaved_write_frame);
    auto _av_packet_unref = GET_AND_CAST_FUNC(av_packet_unref);
    auto _av_packet_free = GET_AND_CAST_FUNC(av_packet_free);

    if (!m_isOpen) {
        m_err.reset();
        m_err.sourceText = "Audio writer must be open before writing."_ba;
        goto failBeforeFrameAlloc;
    }

    depth = av_get_bytes_per_sample(m_smplFmt);
    if (depth <= 0) {
        m_err.reset();
        m_err.sourceText =
            "Could not determine a valid bytes-per-sample (depth) count "
            "for the currently set sample format."_ba;
        goto failBeforeFrameAlloc;
    }

    isPlanar = _av_sample_fmt_is_planar(m_smplFmt);
    // Check if byte count of the input is an integer multiple of the sample
    // byte depth times the number of channels
    multiplier = depth * m_channels;

    smplCount = in.size() / depth;
    smplPerChCount = in.size() / multiplier;
    if (in.size() % multiplier != 0) {
        m_err.reset();
        m_err.sourceText = // TODO
            "Input data's size must be a multiple of the set sample format's "
            "bytes-per-sample (depth) count (of %1) times the channel count "
            "(of %2) (total: %3)."_ba;
        m_err.args.reserve(3);
        m_err.args << QString::number(depth) << QString::number(m_channels)
                   << QString::number(multiplier);
        goto failBeforeFrameAlloc;
    }

    frame = _av_frame_alloc();
    if (!frame) {
        m_err.reset();
        m_err.sourceText =
            "Memory error: failed to initialize the frame for the audio "
            "writing operation."_ba;
        goto failBeforeFrameAlloc;
    }

    frame->format = m_smplFmt;
    frame->sample_rate = m_smplRate;
    frame->nb_samples = smplCount;

    eCode = _av_frame_get_buffer(frame, 0);
    if (eCode < 0) {
        m_err.reset();
        m_err.sourceText =
            "Failed to initialize the audio frame's buffer(s): %1"_ba;
        m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                          .value_or(qTr(UNKNOWN_ERR_CSTR));
        m_isErrorLast = true;
        // Let's not forget to free our resources, to avoid leaks!
        goto failAfterFrameAlloc;
    }

    eCode = _av_frame_make_writable(frame);
    if (eCode < 0) {
        m_err.reset();
        m_err.sourceText =
            "Failed to assert the audio frame's writeability: %1"_ba;
        m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                          .value_or(qTr(UNKNOWN_ERR_CSTR));
        goto failAfterFrameAlloc;
    }

    // Copy samples
    size_t planeByteCount;
    if (isPlanar) {
        /*
         * Memory organization is:
         * Plane 0: {ch0, ch0, ...}
         * Plane 1: {ch1, ch1, ...}
         * ...
         * Plane N: {chN, chN, ...}
         */
        planeByteCount = smplPerChCount * depth;
        for (int ch = 0; ch < m_channels; ch++)
            std::memcpy(frame->data[ch], in.data() + planeByteCount * ch,
                        planeByteCount);
    } else { // Is packed
        /*
         * Memory organization is:
         * Plane 0: {ch0, ch1, ..., chN, ch0, ch1, ...} (all samples)
         */
        planeByteCount = smplCount * depth;
        std::memcpy(frame->data[0], in.data(), planeByteCount);
    }

    // Send written frame to context
    frame->pts = m_pts;
    eCode = _avcodec_send_frame(m_codecCtx, frame);
    if (eCode < 0) {
        m_err.reset();
        m_err.sourceText = "Failed to process/encode audio frame: %1"_ba;
        m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                          .value_or(qTr(UNKNOWN_ERR_CSTR));
        goto failAfterFrameAlloc;
    }

    while (true) {
        eCode = _avcodec_receive_packet(m_codecCtx, packet);
        if (eCode == AVERROR_EOF)
            break;
        else if (eCode < 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to receive encoded audio packet(s): %1"_ba;
            m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                              .value_or(qTr(UNKNOWN_ERR_CSTR));
            goto failAfterPacketCreation;
        }
        packet->stream_index = m_strm->index;
#if 0
        packet->pts = packet->dts = m_pts;
#endif // 0
        _av_packet_rescale_ts(packet, m_codecCtx->time_base, m_strm->time_base);

        eCode = _av_interleaved_write_frame(m_fmtCtx, packet);
        _av_packet_unref(packet);
        if (eCode < 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to write encoded audio packet(s) to output: %1"_ba;
            m_err.args << avStrerrorAsQString(eCode, _av_strerror)
                              .value_or(qTr(UNKNOWN_ERR_CSTR));
            goto failAfterPacketCreation;
        }
    }
    m_pts += frame->nb_samples;

    if (packet)
        _av_packet_free(&packet);
    if (frame)
        _av_frame_free(&frame);
    m_isErrorLast = false;
    SET_IF_PTR(ok, true);
    // Success, all done:
    return;

failAfterPacketCreation:
    if (packet)
        _av_packet_free(&packet);
failAfterFrameAlloc:
    if (frame)
        _av_frame_free(&frame);
failBeforeFrameAlloc:
    m_isErrorLast = true;
    SET_IF_PTR(ok, false);
}

void AudioWriter::closeLockless(bool failOnError, boost::logic::tribool *ok) {
    // using namespace Qt::Literals::StringLiterals;
    if (m_isOpen) {
        SET_IF_PTR(ok, boost::logic::indeterminate)
        return;
    }

    AudioWriterPrivate awp(this);
    int eCode;

    auto _avformat_flush = GET_AND_CAST_FUNC(avformat_flush);
    auto _av_write_trailer = GET_AND_CAST_FUNC(av_write_trailer);
    auto _avcodec_free_context = GET_AND_CAST_FUNC(avcodec_free_context);
    auto _avformat_free_context = GET_AND_CAST_FUNC(avformat_free_context);
    auto _av_strerror = GET_AND_CAST_FUNC(av_strerror);

    static constexpr auto AVFORMAT_FLUSH_FAIL_CSTR =
        "Failed to flush buffered data from audio writer's format context: %1";
    static constexpr auto AV_WRITE_TRAILER_FAIL_CSTR =
        "Failed to write file trailer on the audio writer's output: %1";

    if (m_fmtCtx) {
        eCode = _avformat_flush(m_fmtCtx);
        if (eCode < 0) {
            QString errorStr = avStrerrorAsQString(eCode, _av_strerror)
                                   .value_or(qTr(UNKNOWN_ERR_CSTR));
            qWarning() << qTr(AVFORMAT_FLUSH_FAIL_CSTR).arg(errorStr);
            if (failOnError) {
                m_err.reset();
                m_err.sourceText = QByteArrayLiteral(AVFORMAT_FLUSH_FAIL_CSTR);
                m_err.args << std::move(errorStr);
                m_isErrorLast = true;
                return;
            }
        }
        eCode = _av_write_trailer(m_fmtCtx);
        if (eCode < 0) {
            QString errorStr = avStrerrorAsQString(eCode, _av_strerror)
                                   .value_or(qTr(UNKNOWN_ERR_CSTR));
            qWarning() << qTr(AV_WRITE_TRAILER_FAIL_CSTR).arg(errorStr);
            if (failOnError) {
                m_err.reset();
                m_err.sourceText =
                    QByteArrayLiteral(AV_WRITE_TRAILER_FAIL_CSTR);
                m_err.args << std::move(errorStr);
                m_isErrorLast = true;
                return;
            }
        }
        _avformat_free_context(m_fmtCtx);
        m_fmtCtx = nullptr;
    }

    if (m_codecCtx)
        _avcodec_free_context(&m_codecCtx);

    m_pts = 0;
    m_isErrorLast = m_isOpen = false;
}
} // namespace VvvfSimulator::Generation::Audio

#undef GET_AND_CAST_FUNC
#undef SET_IF_PTR
#undef LOCK_AND_RETURN
#undef DEFAULT_LOCK_PROLOGUE
