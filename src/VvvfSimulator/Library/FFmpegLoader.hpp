#pragma once
#ifndef LIBRARY__FFMPEGLOADER_HPP
#define LIBRARY__FFMPEGLOADER_HPP

// Copyright © 2026 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Library/FFmpegLoader.hpp
// v1.10.0.1

// Packages
#include <QByteArray>
#include <QFileInfo>
#include <QFunctionPointer>
#include <QHash>
#include <QLibrary>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QString>
// Internal
#include "../Util/String.hpp"

namespace VvvfSimulator::Library {
class FFmpegLoader : public QObject {
    Q_OBJECT

  public:
    typedef QHash<QByteArray, QFunctionPointer> FunctionMap;

    explicit FFmpegLoader(QObject *parent = nullptr);
    ~FFmpegLoader() override;

    enum class Libraries { Avutil, Swresample, Avcodec, Avformat };

    // Getters
    QString errorString() const;
    Util::String::TranslatableFmtString errorStringRaw() const;
    QString errorStringUntr() const;
    FunctionMap libavutilFuncCache() const;
    FunctionMap libavcodecFuncCache() const;
    FunctionMap libavformatFuncCache() const;
    FunctionMap libswresampleFuncCache() const;

    QFunctionPointer resolve(const QByteArray &symbol, Libraries from);
    QFunctionPointer resolveAvutil(const QByteArray &symbol);
    QFunctionPointer resolveAvcodec(const QByteArray &symbol);
    QFunctionPointer resolveAvformat(const QByteArray &symbol);
    QFunctionPointer resolveSwresample(const QByteArray &symbol);
    bool load(const QFileInfo &avutilPath, const QFileInfo &swresamplePath,
              const QFileInfo &avcodecPath, const QFileInfo &avformatPath);
    bool isLoaded() const;
    void unload();

  private:
    mutable QMutex m_lock;

    Util::String::TranslatableFmtString m_errStr;

    QLibrary m_libavutil;
    QLibrary m_libswresample;
    QLibrary m_libavcodec;
    QLibrary m_libavformat;

    FunctionMap m_libavutilFuncCache;
    FunctionMap m_libswresampleFuncCache;
    FunctionMap m_libavcodecFuncCache;
    FunctionMap m_libavformatFuncCache;

    unsigned int m_libavutilVer;
    unsigned int m_libswresampleVer;
    unsigned int m_libavcodecVer;
    unsigned int m_libavformatVer;
};
} // namespace VvvfSimulator::Library

#endif // LIBRARY__FFMPEGLOADER_HPP