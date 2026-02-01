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
  #pragma region OneShotPrompts
  //
  // Query one-shot prompts from FFmpeg
  //

  /*
  @brief Attempts to read an FFmpeg executable's supported features,
  and puts the results on the enabled and disabled parameter lists.
  @throws std::runtime_error
  */
  static void getFFmpegFeatures(const std::filesystem::path &path,
                                   QByteArrayList &enabled,
                                   QByteArrayList &disabled);
  /*
  @brief Attempts to read an FFmpeg executable's version information.
  @throws std::runtime_error
  */
  static QByteArray getFFmpegVersion(const std::filesystem::path &path);

  enum class HelpLength {
    Basic,
    Long,
    Full
  };

  /*
  @brief Attempts to read an FFmpeg encoder's options.
  @returns If FFmpeg recognizes the name, the returned string will be the help
  text for the option. Otherwise, this will return an error string like
  "Codec '[name]' is not recognized by FFmpeg".
  @throws std::runtime_error
  */
  static QByteArray getFFmpegHelpEncoder(const std::filesystem::path &path,
                                         const QStringView &name,
                                         HelpLength length = HelpLength::Basic);
  
  /*
  @brief Attempts to read an FFmpeg format's options.
  @returns If FFmpeg recognizes the name, the returned string will be the help
  text for the option. Otherwise, this will return an error string like
  "Codec '[name]' is not recognized by FFmpeg".
  @throws std::runtime_error
  */
  static QByteArray getFFmpegHelpFormat(const std::filesystem::path &path,
                                        const QStringView &name,
                                        HelpLength length = HelpLength::Basic);

#pragma endregion

  std::filesystem::path path() const;
};
} // namespace VvvfSimulator::Generation::FFmpegProcess