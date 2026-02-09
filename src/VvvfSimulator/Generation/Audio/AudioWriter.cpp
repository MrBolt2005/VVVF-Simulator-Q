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
    AudioWriterPrivate awp(this);

    if (m_isOpen) {
        SET_IF_PTR(ok, boost::logic::indeterminate)
        return;
    }

    const QUrl *currUrl = url ? url : &m_url;

    // Fetch cached functions
    auto _avformat_alloc_context = GET_AND_CAST_FUNC(avformat_alloc_context);
    /*
     * avformat_network_init is going to be deprecated in the foreseeable
     * future, so we should be safe skipping it~
     */
    auto _av_strdup = GET_AND_CAST_FUNC(av_strdup);
    auto _avformat_free_context = GET_AND_CAST_FUNC(avformat_free_context);
    auto _avcodec_alloc_context3 = GET_AND_CAST_FUNC(avcodec_alloc_context3);
    auto _avcodec_free_context = GET_AND_CAST_FUNC(avcodec_free_context);
    auto _avformat_new_stream = GET_AND_CAST_FUNC(avformat_new_stream);
    auto _av_dict_copy = GET_AND_CAST_FUNC(av_dict_copy);
    auto _av_dict_set = GET_AND_CAST_FUNC(av_dict_set);
    auto _av_dict_free = GET_AND_CAST_FUNC(av_dict_free);
    auto _av_strerror = GET_AND_CAST_FUNC(av_strerror);

    AVDictionary *optsCopy = nullptr;
    // Util::String::TranslatableFmtString error;
    try {
        // Initialize AVFormatContext
        m_fmtCtx = _avformat_alloc_context();
        if (!m_fmtCtx) {
            m_err.reset();
            m_err.sourceText =
                "Memory error: failed to initialize the audio writer's format context."_ba;
            SET_IF_PTR(ok, false);
            return;
        }

        QByteArray urlCString = currUrl->isLocalFile()
                                    ? currUrl->toLocalFile().toLocal8Bit()
                                    : currUrl->url().toUtf8();
        m_fmtCtx->url = _av_strdup(urlCString);
        if (!m_fmtCtx->url) {
            m_err.reset();
            m_err.sourceText =
                "Memory error: failed to set URL in the audio writer's format context."_ba;
            m_isErrorLast = true;
            SET_IF_PTR(ok, false);
            return;
        }

        // Initialize AVCodecContext
        m_codecCtx = _avcodec_alloc_context3(m_codec);
        if (!m_codecCtx) {
            m_err.reset();
            m_err.sourceText =
                "Memory error: failed to initialize the audio writer's codec context."_ba;
            m_isErrorLast = true;
            SET_IF_PTR(ok, false);
            return;
        }
        m_codecCtx->bit_rate = m_bitRate;
        // TODO: Correct this following line
        m_codecCtx->bit_rate_tolerance =
            std::abs(m_brRange.first - m_brRange.second);
        // m_codecCtx->codec = m_codec;
        m_codecCtx->flags = m_flags;
        m_codecCtx->flags2 = m_flags2;
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
        if (eCode != 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to set up the audio writer's options: %1"_ba;
            m_err.args << {avStrerrorAsQString(eCode, _av_strerror)
                     .value_or(qTr(UNKNOWN_ERR_CSTR))});
            m_isErrorLast = true;
            SET_IF_PTR(ok, false);
            return;
        }

        eCode = avformat_init_output(m_fmtCtx, &optsCopy);
        if (eCode < 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to initialize the audio writer's output: %1"_ba;
            m_err.args << {avStrerrorAsQString(eCode, _av_strerror)
                     .value_or(qTr(UNKNOWN_ERR_CSTR))});
            m_isErrorLast = true;
            SET_IF_PTR(ok, false);
            return;
        } else if (eCode == AVSTREAM_INIT_IN_WRITE_HEADER) {
            qDebug().nospace()
                << "avformat_init_output() -> AVSTREAM_INIT_IN_WRITE_HEADER ("
                << eCode << ')';
        } else if (eCode == AVSTREAM_INIT_IN_INIT_OUTPUT) {
            qDebug().nospace()
                << "avformat_init_output() -> AVSTREAM_INIT_IN_INIT_OUTPUT ("
                << eCode << ')';
        }
        eCode = avformat_write_header(m_fmtCtx, nullptr);
        if (eCode < 0) {
            m_err.reset();
            m_err.sourceText =
                "Failed to write the file header on the audio writer's output: %1"_ba;
            m_err.args << {avStrerrorAsQString(eCode, _av_strerror)
                               .value_or(qTr(UNKNOWN_ERR_CSTR))};
            m_isErrorLast = true;
            SET_IF_PTR(ok, false);
            return;
        } else if (eCode == AVSTREAM_INIT_IN_WRITE_HEADER) {
            qDebug().nospace()
                << "avformat_write_header() -> AVSTREAM_INIT_IN_WRITE_HEADER ("
                << eCode << ')';
        } else if (eCode == AVSTREAM_INIT_IN_INIT_OUTPUT) {
            qDebug().nospace()
                << "avformat_write_header() -> AVSTREAM_INIT_IN_INIT_OUTPUT ("
                << eCode << ')';
        }

        m_isErrorLast = false;
        m_isOpen = true;
        SET_IF_PTR(ok, true);
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
        m_err.args << {ex.what()};
        m_isErrorLast = true;
        SET_IF_PTR(ok, false);
    }
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
        if (eCode != 0) {
            QString errorStr = avStrerrorAsQString(eCode, _av_strerror)
                                   .value_or(qTr(UNKNOWN_ERR_CSTR));
            qWarning() << qTr(AVFORMAT_FLUSH_FAIL_CSTR).arg(errorStr);
            if (failOnError) {
                m_err.reset();
                m_err.sourceText = QByteArrayLiteral(AVFORMAT_FLUSH_FAIL_CSTR);
                m_err.args << {std::move(errorStr)};
                m_isErrorLast = true;
                return;
            }
        }
        eCode = _av_write_trailer(m_fmtCtx);
        if (eCode != 0) {
            QString errorStr = avStrerrorAsQString(eCode, _av_strerror)
                                   .value_or(qTr(UNKNOWN_ERR_CSTR));
            qWarning() << qTr(AV_WRITE_TRAILER_FAIL_CSTR).arg(errorStr);
            if (failOnError) {
                m_err.reset();
                m_err.sourceText =
                    QByteArrayLiteral(AV_WRITE_TRAILER_FAIL_CSTR);
                m_err.args << {std::move(errorStr)};
                m_isErrorLast = true;
                return;
            }
        }
        _avcodec_free_context(&m_codecCtx);
        _avformat_free_context(m_fmtCtx);
        m_isErrorLast = m_isOpen = false;
    }
} // namespace VvvfSimulator::Generation::Audio

#undef GET_AND_CAST_FUNC
#undef SET_IF_PTR
#undef LOCK_AND_RETURN
#undef DEFAULT_LOCK_PROLOGUE
