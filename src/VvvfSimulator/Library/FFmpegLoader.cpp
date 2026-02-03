#include "FFmpegLoader.hpp"

// Standard Library
#include <filesystem>
// Packages
#include <QMutexLocker>

#define DEFAULT_LOCK_PROLOGUE QMutexLocker locker(&m_lock);

namespace VvvfSimulator::Library {
FFmpegLoader::FFmpegLoader(QObject *parent)
    : QObject(parent)
    , m_lock()
    , m_errStr()
    , m_libavutil(this)
    , m_libswresample(this)
    , m_libavcodec(this)
    , m_libavformat(this)
    , m_libavutilFuncCache()
    , m_libswresampleFuncCache()
    , m_libavcodecFuncCache()
    , m_libavformatFuncCache()
    , m_libavutilVer(0)
    , m_libswresampleVer(0)
    , m_libavcodecVer(0)
    , m_libavformatVer(0) {
    DEFAULT_LOCK_PROLOGUE

    m_libavutil.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    m_libswresample.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    m_libavcodec.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    m_libavformat.setLoadHints(QLibrary::ExportExternalSymbolsHint);
}

FFmpegLoader::~FFmpegLoader() {
    unload();
}

namespace {
// TODO: Populate this later
constexpr unsigned minAvutilVersion = 8 << 8;
constexpr unsigned maxAvutilVersion = 9 << 8 - 1;

constexpr unsigned minSwresampleVersion = 1 << 8;
constexpr unsigned maxSwresampleVersion = 2 << 8 - 1;

QFunctionPointer resolveDetail(const QByteArray &sym, QLibrary &lib,
                               FFmpegLoader::FunctionMap &cache,
                               Util::String::TranslatableFmtString &err) {
    auto maybeRetVal = cache.find(sym);
    if (maybeRetVal != cache.end())
        return maybeRetVal.value();

    QFunctionPointer ptr = lib.resolve(sym);
    if (ptr)
        cache.emplace(sym, ptr);
    else {
        Util::String::TranslatableFmtString err2;
        err2.sourceText = QByteArrayLiteral("%1");
        err2.args = {{lib.errorString()}};
        err = std::move(err2);
    }
    return ptr;
}

void unloadDetail(QLibrary &lib, FFmpegLoader::FunctionMap *cache) {
    bool didLibUnload = lib.unload();
    Q_ASSERT(didLibUnload);
    Q_UNUSED(didLibUnload);
    if (cache)
        cache->clear();
}
} // namespace

QString FFmpegLoader::errorString() const {
    DEFAULT_LOCK_PROLOGUE
    return m_errStr.makeUntrString();
}

Util::String::TranslatableFmtString FFmpegLoader::errorStringRaw() const {
    DEFAULT_LOCK_PROLOGUE
    return m_errStr;
}

QString FFmpegLoader::errorStringTr() const {
    DEFAULT_LOCK_PROLOGUE
    return m_errStr.makeTrString();
}

FFmpegLoader::FunctionMap FFmpegLoader::libavutilFuncCache() const {
    DEFAULT_LOCK_PROLOGUE
    return m_libavutilFuncCache;
}

FFmpegLoader::FunctionMap FFmpegLoader::libavcodecFuncCache() const {
    DEFAULT_LOCK_PROLOGUE
    return m_libavcodecFuncCache;
}

FFmpegLoader::FunctionMap FFmpegLoader::libavformatFuncCache() const {
    DEFAULT_LOCK_PROLOGUE
    return m_libavformatFuncCache;
}

FFmpegLoader::FunctionMap FFmpegLoader::libswresampleFuncCache() const {
    DEFAULT_LOCK_PROLOGUE
    return m_libswresampleFuncCache;
}

QFunctionPointer FFmpegLoader::resolve(const QByteArray &symbol,
                                       FFmpegLoader::Libraries from) {
    switch (from) {
    case Libraries::Avutil:
        return resolveAvutil(symbol);
        break;
    case Libraries::Swresample:
        return resolveSwresample(symbol);
        break;
    case Libraries::Avcodec:
        return resolveAvcodec(symbol);
        break;
    default: // case Libraries::Avformat:
        return resolveAvformat(symbol);
    }
}

QFunctionPointer FFmpegLoader::resolveAvutil(const QByteArray &symbol) {
    DEFAULT_LOCK_PROLOGUE
    return resolveDetail(symbol, m_libavutil, m_libavutilFuncCache, m_errStr);
}

QFunctionPointer FFmpegLoader::resolveSwresample(const QByteArray &symbol) {
    DEFAULT_LOCK_PROLOGUE
    return resolveDetail(symbol, m_libswresample, m_libswresampleFuncCache,
                         m_errStr);
}

QFunctionPointer FFmpegLoader::resolveAvcodec(const QByteArray &symbol) {
    DEFAULT_LOCK_PROLOGUE
    return resolveDetail(symbol, m_libavcodec, m_libavcodecFuncCache, m_errStr);
}

QFunctionPointer FFmpegLoader::resolveAvformat(const QByteArray &symbol) {
    DEFAULT_LOCK_PROLOGUE
    return resolveDetail(symbol, m_libavformat, m_libavformatFuncCache,
                         m_errStr);
}

bool FFmpegLoader::load(const QFileInfo &avutilPath,
                        const QFileInfo &swresamplePath,
                        const QFileInfo &avcodecPath,
                        const QFileInfo &avformatPath) {
    namespace fs = std::filesystem;
    using namespace Qt::Literals::StringLiterals;

    DEFAULT_LOCK_PROLOGUE
    static constexpr bool failVal = false;
    Util::String::TranslatableFmtString error;

    if (!avutilPath.isFile()) {
        error.sourceText =
            QByteArrayLiteral("The given libavutil path (%1) is invalid: "
                              "it must be of an existing and non-empty file.");
        error.args = {{avutilPath.filePath()}};
        m_errStr = std::move(error);
        return failVal;
    }

    if (!swresamplePath.isFile()) {
        error.sourceText =
            QByteArrayLiteral("The given libswresample path (%1) is invalid: "
                              "it must be of an existing and non-empty file.");
        error.args = {{swresamplePath.filePath()}};
        m_errStr = std::move(error);
        return failVal;
    }

    if (!avcodecPath.isFile()) {
        error.sourceText =
            QByteArrayLiteral("The given libavcodec path (%1) is invalid: "
                              "it must be of an existing and non-empty file.");
        error.args = {{avcodecPath.filePath()}};
        m_errStr = std::move(error);
        return failVal;
    }

    if (!avformatPath.isFile()) {
        error.sourceText =
            QByteArrayLiteral("The given libavformat path (%1) is invalid: "
                              "it must be of an existing and non-empty file.");
        error.args = {{avformatPath.filePath()}};
        m_errStr = std::move(error);
        return failVal;
    }

    fs::path avutilParent = avutilPath.filesystemPath();
    fs::path swresampleParent = swresamplePath.filesystemPath();
    fs::path avcodecParent = avcodecPath.filesystemPath();
    fs::path avformatParent = avformatPath.filesystemPath();
    bool areParentsDifferent =
        !fs::equivalent(avutilParent, swresampleParent) ||
        !fs::equivalent(avcodecParent, swresampleParent) ||
        !fs::equivalent(avcodecParent, avformatParent);

    if (areParentsDifferent) {
        error.sourceText =
            QByteArrayLiteral("The given library paths are invalid: they must "
                              "be all in the same directory.");
        m_errStr = std::move(error);
        return failVal;
    }

    /*
    enum LoadHelperSel {
        SelectAvutil,
        SelectSwresample,
        SelectAvcodec,
        SelectAvformat
    };
    */
    unsigned int avutilVerIn, swresampleVerIn, avcodecVerIn, avformatVerIn;
    static const auto libLoadHelper = [&](const QByteArray &versionSym,
                                          Libraries sel) {
        using namespace Qt::Literals::StringLiterals;

        // Which library to handle?
        QLibrary *lib;
        FunctionMap *cache;
        unsigned int minVer;
        QString libName;
        QString filePath;
        unsigned int *verOut;

        switch (sel) {
        case Libraries::Avutil:
            lib = &m_libavutil;
            cache = &m_libavutilFuncCache;
            minVer = minAvutilVersion;
            libName = u"libavutil"_s;
            filePath = avutilPath.filePath();
            verOut = &avutilVerIn;
            break;
        case Libraries::Swresample:
            lib = &m_libswresample;
            cache = &m_libswresampleFuncCache;
            minVer = minSwresampleVersion;
            libName = u"libswresample"_s;
            filePath = swresamplePath.filePath();
            verOut = &swresampleVerIn;
            break;
        case Libraries::Avcodec:
            lib = &m_libavcodec;
            cache = &m_libavcodecFuncCache;
            minVer = 0;
            libName = u"libavcodec"_s;
            filePath = avcodecPath.filePath();
            verOut = &avcodecVerIn;
            break;
        default: // case Libraries::Avformat:
            lib = &m_libavformat;
            cache = &m_libavformatFuncCache;
            minVer = 0;
            libName = u"libavformat"_s;
            filePath = avformatPath.filePath();
            verOut = &avformatVerIn;
        }

        // Try loading the library
        Q_ASSERT(cache->isEmpty());
        lib->setFileName(filePath);
        if (!lib->load()) {
            error.sourceText = QByteArrayLiteral(
                // "Failed to load the libavutil library: %1"
                "%1");
            error.args = {{lib->errorString()}};
            m_errStr = std::move(error);

            switch (sel) {
            // case Libraries::Avutil: Nothing
            case Libraries::Swresample:
                unloadDetail(m_libavutil, nullptr);
                break;
            case Libraries::Avcodec:
                unloadDetail(m_libswresample, nullptr);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
                break;
            case Libraries::Avformat:
                unloadDetail(m_libavcodec, nullptr);
                unloadDetail(m_libswresample, &m_libswresampleFuncCache);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
                break;
            }

            return false;
        }

        // Check for version function; resolveDetail also populates
        // cache if successful
        auto versionGetter = (unsigned (*)(void))resolveDetail(
            versionSym, *lib, *cache, m_errStr);
        if (!versionGetter) {
            switch (sel) {
            case Libraries::Avutil:
                unloadDetail(m_libavutil, nullptr);
                break;
            case Libraries::Swresample:
                unloadDetail(m_libswresample, nullptr);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
                break;
            case Libraries::Avcodec:
                unloadDetail(m_libavcodec, nullptr);
                unloadDetail(m_libswresample, &m_libswresampleFuncCache);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
                break;
            default: // case Libraries::Avformat:
                unloadDetail(m_libavformat, nullptr);
                unloadDetail(m_libavcodec, &m_libavcodecFuncCache);
                unloadDetail(m_libswresample, &m_libswresampleFuncCache);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
            }

            return false;
        }

        // Know which is the desired library's version
        auto version = versionGetter();

        // Check if version is correct
        if (version < minVer) {
            error.sourceText =
                "%1 version is too old: requires >= 0x%2, but got 0x%3."_ba;
            error.args = {{std::move(libName)},
                          {QString::number(minVer, 16)},
                          {QString::number(version, 16)}};
            m_errStr = std::move(error);

            switch (sel) {
            case Libraries::Avutil:
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
                break;
            case Libraries::Swresample:
                unloadDetail(m_libswresample, &m_libswresampleFuncCache);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
                break;
            case Libraries::Avcodec:
                unloadDetail(m_libavcodec, &m_libavcodecFuncCache);
                unloadDetail(m_libswresample, &m_libswresampleFuncCache);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
                break;
            default: // case Libraries::Avformat:
                unloadDetail(m_libavformat, &m_libavformatFuncCache);
                unloadDetail(m_libavcodec, &m_libavcodecFuncCache);
                unloadDetail(m_libswresample, &m_libswresampleFuncCache);
                unloadDetail(m_libavutil, &m_libavutilFuncCache);
            }

            return false;
        }

        *verOut = version;
        // All fine, return success
        return true;
    };

    if (!libLoadHelper("avutil_version"_ba, Libraries::Avutil))
        return false;
    if (!libLoadHelper("swresample_version"_ba, Libraries::Swresample))
        return false;
    if (!libLoadHelper("avcodec_version"_ba, Libraries::Avcodec))
        return false;
    if (!libLoadHelper("avformat_version"_ba, Libraries::Avformat))
        return false;

    m_libavutilVer = avutilVerIn;
    m_libswresampleVer = swresampleVerIn;
    m_libavcodecVer = avcodecVerIn;
    m_libavformatVer = avformatVerIn;

    // Report success
    return true;
}

bool FFmpegLoader::isLoaded() const {
    DEFAULT_LOCK_PROLOGUE
    return m_libavformat.isLoaded();
}

void FFmpegLoader::unload() {
    DEFAULT_LOCK_PROLOGUE
    if (m_libavformat.isLoaded()) {
        unloadDetail(m_libavformat, &m_libavformatFuncCache);
        unloadDetail(m_libavcodec, &m_libavcodecFuncCache);
        unloadDetail(m_libswresample, &m_libswresampleFuncCache);
        unloadDetail(m_libavutil, &m_libavutilFuncCache);

        m_libavutilVer = 0;
        m_libswresampleVer = 0;
        m_libavcodecVer = 0;
        m_libavformatVer = 0;
    }
}
} // namespace VvvfSimulator::Library

#undef DEFAULT_LOCK_PROLOGUE