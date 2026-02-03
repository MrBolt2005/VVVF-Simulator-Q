#include "IWriter.hpp"

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Library/FFmpegLoader.cpp
 * v1.10.0.1
 */

// Packages
#include <libavutil/error.h>
#include <QDebug>
#include <QMutexLocker>

#define DEFAULT_LOCK_PROLOGUE QMutexLocker locker(&m_lock);
#define LOCK_AND_RETURN(x) DEFAULT_LOCK_PROLOGUE return x;

namespace VvvfSimulator::Generation {
void IWriter::close(bool failOnError, boost::logic::tribool *ok){
    LOCK_AND_RETURN(closeLockless(failOnError, ok))}

IWriter::IWriter(QObject *parent)
    : QObject(parent)
    , m_lock()
    , m_err()
    , m_loader()
    , m_isErrorLast()
    , m_isOpen()
    , m_localFuncCache() {}

IWriter::~IWriter() = default;

QString IWriter::avStrerrorAsQString(int err, IWriter::AvStrerrorType func) {
    QString retVal;
    if (func) {
        std::array<char, AV_ERROR_MAX_STRING_SIZE + 1> buf;
        std::memset(buf.data(), 0, buf.size() * sizeof(buf[0]));
        int eCode = func(err, buf.data(), buf.size());
        if (eCode != 0)
            qWarning() << qTr("av_strerror error: %1").arg(eCode);
        retVal = QString(buf.data());
    } else {
        qCritical() << qTr("The passed (%1) function pointer is null.")
                           .arg("av_strerror");
    }
    return retVal;
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

boost::logic::tribool IWriter::setLoaderDetail(
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
            auto &sel = it.second;
            auto funcPtr =
                resolveOrSetErrorLockless(sym, sel, *loaderCopy.get());
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

#undef DEFAULT_LOCK_PROLOGUE