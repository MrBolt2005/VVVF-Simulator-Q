#pragma once
#ifndef GENERATION__WRITER_HPP
#define GENERATION__WRITER_HPP

/*
 * Copyright © 2026 VvvfGeeks, VVVF Systems
 * SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
 *
 * Library/FFmpegLoader.hpp
 * v1.10.0.1
 */

// Standard Library
#include <optional>
// Packages
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QUrl>
#include <boost/logic/tribool.hpp>
// Internal
#include "../Library/FFmpegLoader.hpp"

namespace VvvfSimulator::Generation {
class IWriter : public QObject {
    Q_OBJECT

  public:
    using FFmpegLoader = Library::FFmpegLoader;
    using LoaderPtr = std::shared_ptr<FFmpegLoader>;
    using CLoaderPtr = std::shared_ptr<const FFmpegLoader>;

    QString errorString() const;
    Util::String::TranslatableFmtString errorStringRaw() const;
    QString errorStringUntr() const;
    bool isErrorLast() const;
    CLoaderPtr loader() const;
    QUrl url() const;

    bool isLoaderSet() const;

  public slots:
    // Setters
    void setUrl(const QUrl &url, boost::logic::tribool *ok = nullptr);

    void close(bool failOnError, boost::logic::tribool *ok = nullptr);

    virtual void open(const QUrl *url = nullptr,
                      boost::logic::tribool *ok = nullptr) = 0;

    /*
     * @brief Object must be closed (!isOpen()), or this function will fail.
     * @param loader The object's FFmpeg library loader will be set to this.
     * If null, effectively just unsets the current loader.
     * @param ok If not null, will be set to an exit code when the function
     * is finished.
     * @returns If ok is not null, it's set to:
     * true if fully successful,
     * indeterminate if already set to the «loader» parameter - or, therefore,
     * if already unset upon trying to do so -,
     * and false in case of failure.
     * Possible failure causes are object open upon call or symbol resolution
     * failure on the FFmpegLoader side.
     */
    virtual void setLoader(LoaderPtr loader,
                           boost::logic::tribool *ok = nullptr) = 0;

  protected:
    using ErrorType = Util::String::TranslatableFmtString;
    using FunctionMap = FFmpegLoader::FunctionMap;

    explicit IWriter(QObject *parent = nullptr);
    ~IWriter() override;

#pragma region Convenience Parsers
    using AvStrerrorType = int (*)(int, char *, size_t);
    static std::optional<QString>
    avStrerrorAsQString(int err, const AvStrerrorType func);
#pragma endregion

#pragma region Loading Implementation
    virtual void closeLockless(bool failOnError, boost::logic::tribool *ok) = 0;
    QFunctionPointer resolveOrSetErrorLockless(const QByteArray &symbol,
                                               FFmpegLoader::Libraries from,
                                               FFmpegLoader &loader);
    using EagerLoadTableEntry = QPair<QByteArray, FFmpegLoader::Libraries>;
    boost::logic::tribool
    setLoaderDetailLockless(LoaderPtr loader,
                            const std::span<EagerLoadTableEntry> &loadTable);
#pragma endregion

#pragma region Members
    mutable QMutex m_lock;
    ErrorType m_err;
    LoaderPtr m_loader;
    bool m_isErrorLast, m_isOpen;
    FunctionMap m_localFuncCache;
    QUrl m_url;
#pragma endregion
};
} // namespace VvvfSimulator::Generation

#endif // GENERATION__WRITER_HPP