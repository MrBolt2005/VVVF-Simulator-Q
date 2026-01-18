#pragma once

// Copyright © 2026 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Generation/FFmpegProcess/ProcessData.hpp
// v1.10.0.0

// Standard Library
#include <filesystem>
#include <optional>
// Packages
#include <QByteArray>
#include <QProcess>
#include <QString>

namespace VvvfSimulator::Generation::FFmpegProcess {
class ProcessData {
  Q_GADGET

  std::filesystem::path m_path;
  QStringList m_userArguments;

public:
  /*
  @brief Attempts to read an FFmpeg executable's supported features,
  and puts the results on the enabled and disabled parameter lists.
  @throws std::runtime_error
  */
  static void tryGetFFmpegFeatures(const std::filesystem::path &path,
                                   QByteArrayList &enabled,
                                   QByteArrayList &disabled);
  /*
  @brief Attempts to read an FFmpeg executable's version information.
  @throws std::runtime_error
  */
  static QByteArray tryGetFFmpegVersion(const std::filesystem::path &path);

  std::filesystem::path path() const;
};
} // namespace VvvfSimulator::Generation::FFmpegProcess