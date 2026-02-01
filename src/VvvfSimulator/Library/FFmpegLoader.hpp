#pragma once

// Copyright © 2026 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Library/FFmpegLoader.hpp
// v1.10.0.0

// Standard Library
#include <filesystem>
// Packages
#include <QByteArray>
#include <QFunctionPointer>
#include <QLibrary>
#include <QPair>
#include <QVariant>
// Internal
#include "../Util/String.hpp"

namespace VvvfSimulator::Library::FFmpegLoader {
typedef QHash<QByteArray, QFunctionPointer> FunctionMap;

#pragma region Loading
bool isLoaded();

/*
@brief Tries to load the FFmpeg library from the desired path.
@returns A pair where element 1 is the version code if fetched, and element 2,
whether the library was loaded and the version was valid.
*/
QPair<QByteArray, bool> load(const QString &fileName,
                             const QVariant *version = nullptr);

bool unload();

#pragma region Error
#pragma endregion

#pragma endregion

#pragma region Cache
FunctionMap currentCache();

#pragma endregion

#pragma region Resolving
/*
@brief Tries to resolve a function from the loaded library.
@returns If successful, the resolved function pointer, otherwise nullptr.
@throws Sets error() and wasLastError().
*/
QFunctionPointer resolve(const QByteArray &symbol);

#pragma endregion
} // namespace VvvfSimulator::Library::FFmpegLoader