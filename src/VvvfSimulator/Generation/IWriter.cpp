#include "IWriter.hpp"

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Library/FFmpegLoader.cpp
 * v1.10.0.1
 */

// Packages
#include <QDebug>
#include <QMutexLocker>
#include <libavutil/error.h>

#define DEFAULT_LOCK_PROLOGUE QMutexLocker locker(&m_lock);
#define LOCK_AND_RETURN(x) DEFAULT_LOCK_PROLOGUE return x;
#define SET_IF_PTR(ptr, val)                                                   \
    if (ptr)                                                                   \
        *ptr = val;

namespace VvvfSimulator::Generation {

bool IWriter::isErrorLast() const {LOCK_AND_RETURN(m_isErrorLast)}

IWriter::CLoaderPtr IWriter::loader() const {LOCK_AND_RETURN(m_loader)}

QUrl IWriter::url() const {
    LOCK_AND_RETURN(m_url)
}

void IWriter::setUrl(const QUrl &url, boost::logic::tribool *ok) {
    DEFAULT_LOCK_PROLOGUE

    if (url == m_url) {
        SET_IF_PTR(ok, boost::logic::indeterminate)
        return;
    }

    using namespace Qt::Literals::StringLiterals;
    if (m_isOpen) {
        Util::String::TranslatableFmtString error;
        error.sourceText = "Could not set file path: writer must be closed."_ba;
        m_err = std::move(error);
        SET_IF_PTR(ok, false)
        return;
    }

    m_url = url;
    SET_IF_PTR(ok, true)
}

void IWriter::close(bool failOnError, boost::logic::tribool *ok){
    LOCK_AND_RETURN(closeLockless(failOnError, ok))}

IWriter::IWriter(QObject *parent)
    : QObject(parent)
    , m_lock()
    , m_err()
    , m_loader()
    , m_isErrorLast()
    , m_isOpen()
    , m_localFuncCache()
    , m_url() {}

IWriter::~IWriter() {
    close(false);
}

std::optional<QString>
IWriter::avStrerrorAsQString(int err, const IWriter::AvStrerrorType func) {
    if (func) {
        std::array<char, AV_ERROR_MAX_STRING_SIZE + 1> buf;
        std::memset(buf.data(), 0, buf.size() * sizeof(buf[0]));
        int eCode = func(err, buf.data(), buf.size());
        if (eCode != 0)
            qWarning() << qTr("av_strerror error: %1").arg(eCode);
        return QString(buf.data());
    } // else
    qCritical()
        << qTr("The passed (%1) function pointer is null.").arg("av_strerror");
    return std::nullopt;
}

QFunctionPointer
IWriter::resolveOrSetErrorLockless(const QByteArray &symbol,
                                   FFmpegLoader::Libraries from,
                                   FFmpegLoader &loader) {
    QFunctionPointer retVal = loader.resolve(symbol, from);
    if (!retVal) {
        m_err = loader.errorStringRaw();
        m_isErrorLast = true;
    } else
        m_isErrorLast = false;
    return retVal;
}

boost::logic::tribool IWriter::setLoaderDetailLockless(
    LoaderPtr loader,
    const std::span<IWriter::EagerLoadTableEntry> &loadTable) {
    if (m_loader == loader) {
        qDebug() << qTr("FFmpeg loader is already set to: 0x%1")
                        .arg(std::size_t(loader.get()), 16);
        m_isErrorLast = false;
        return boost::logic::indeterminate;
    }

    using namespace Qt::Literals::StringLiterals;

    Util::String::TranslatableFmtString error;

    if (m_isOpen) {
        error.sourceText =
            "Could not set/unset FFmpeg loader: writer must be closed."_ba;

        m_err = std::move(error);
        m_isErrorLast = true;
        return false;
    }

    LoaderPtr loaderCopy(loader);
    FunctionMap cache;

    if (loaderCopy) {
        cache.reserve(loadTable.size());
        for (const auto &it : loadTable) {
            auto &sym = it.first;
            auto &from = it.second;
            auto funcPtr =
                resolveOrSetErrorLockless(sym, from, *loaderCopy.get());
            if (!funcPtr)
                return false;
            cache.emplace(sym, funcPtr);
        }
    }
    m_loader = std::move(loaderCopy);
    m_localFuncCache = std::move(cache);

    m_isErrorLast = false;
    return true;
}
} // namespace VvvfSimulator::Generation

#undef SET_IF_PTR
#undef LOCK_AND_RETURN
#undef DEFAULT_LOCK_PROLOGUE