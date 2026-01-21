#pragma once

// Copyright © 2026 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Generation/Audio/AudioWriter.hpp
// v1.10.0.0

// Standard Library
#include <cinttypes>
#include <filesystem>
#include <optional>
#include <utility>
// Packages
#include <QByteArrayView>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QString>
#include <libavformat/avformat.h>
// Internal
#include "../../Util/String.hpp"
#include "../../Util/Thread.hpp"
#include "Options.hpp"

namespace VvvfSimulator::Generation::Audio {
using namespace FFmpegProcess;
using namespace FFmpegProcess::Options;

class AudioWriter : public QObject {
  Q_OBJECT

  // Should not be implemented
  explicit AudioWriter(QObject *parent = nullptr);

public:
  AudioWriter(const std::filesystem::path &fileName, int64_t bitRate,
              const QPair<int64_t, int64_t> &bitRateRange,
              const QStringView &codecName, int channels,
              const QStringView &sampleFormatName,
              QObject *parent = nullptr);
  ~AudioWriter() override;

  // Getters
  int64_t bitRate() const;
  QPair<int64_t, int64_t> bitRateRange() const;
  std::optional<ChannelLayoutOptions> channelLayout() const;
  int channels() const;
  std::optional<EncoderOptions> codec() const;
  QString codecName() const;
  QString errorString() const;
  Util::String::TranslatableFmtString errorStringRaw() const;
  std::filesystem::path fileName() const;
  int flags() const;
  int flags2() const;
  QMap<QString, QString> inputOption() const;
  bool isOpen() const;
  QMap<QString, QString> outputOption() const;
  std::optional<ProcessData> process() const;
  std::optional<SampleFormatOptions> sampleFormat() const;
  QString sampleFormatName() const;
  int sampleRate() const;
  int strict() const;

public slots:
  // Setters

  void close();
  bool open();
  void writeSamples(const std::span<const char> &data);

signals:
  void errorOccurred();
  void samplesWritten(std::size_t samples, std::size_t bytes);

private:
  Util::Thread::MutexAPtrWrapper m_lock;

  // Fundamental attributes
  std::filesystem::path m_fileName;
  int64_t m_br;
  QPair<int64_t, int64_t> m_brr;
  QString m_encName;
  int m_ch;
  QString m_sampleFmtName;

  // Optional attributes
  QString m_chLytName;
  int m_flags, m_flags2;
  QMap<QString, QString> m_inOpt, m_outOpt;

  // Internal work attributes
  EncoderOptions m_enc;
  Util::String::TranslatableFmtString m_err;
  SampleFormatOptions m_sampleFmt;
  ChannelLayoutOptions m_chLyt;
  QProcess m_proc;

  constexpr auto sizeOf() const noexcept { return sizeof(*this); }
};
} // namespace VvvfSimulator::Generation::Audio