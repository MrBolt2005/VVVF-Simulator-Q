#pragma once

// Copyright © 2026 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Generation/FFmpegProcess/Options.hpp
// v1.10.0.0

// Standard Library
#include <optional>
#include <set>
// Packages
#include <QMap>
#include <QObject>
// Internal
#include "../Outcome.hpp"
#include "ProcessData.hpp"

namespace VvvfSimulator::Generation::FFmpegProcess::Options {
class EncoderOptions {
  Q_GADGET

  friend class EncoderOptionsPrivate;

public:
  enum class EncoderType { Video, Audio, Subtitle };

  /*
  @brief Tries to read the list of encoders supported by an FFmpeg
  executable.
  @returns The list of available encoders.
  @throws std::bad_alloc, std::runtime_error...
  */
  static std::set<EncoderOptions>
  loadAllFromProcess(const FFmpegProcess::ProcessData &proc);
  static std::optional<EncoderOptions>
  loadFromProcess(const FFmpegProcess::ProcessData &proc,
                  const QByteArrayView &name);

  QByteArray name() const;
  QByteArray description() const;
  EncoderType type() const;
  bool supportsFrameLevelMultiThreading() const;
  bool supportsSliceLevelMultiThreading() const;
  bool experimental() const;
  bool supportsDrawHorizontalBand() const;
  bool supportsDRM1() const;

  /*
  @brief Strong ordering comparator
  */
  bool operator<(const EncoderOptions &rhs) const;

private:
  QByteArray m_name, m_desc;
  EncoderType m_type;
  int m_frameLevelMT : 1;
  int m_sliceLevelMT : 1;
  int m_experimental : 1;
  int m_drawHorizBand : 1;
  int m_drm1 : 1;
};

class FormatOptions {
  Q_GADGET

  friend class FormatOptionsPrivate;

public:
  /*
  @brief Tries to read the list of encoders supported by an FFmpeg
  executable.
  @returns The list of available encoders.
  @throws std::bad_alloc, std::runtime_error...
  */
  static std::set<FormatOptions>
  loadAllFromProcess(const FFmpegProcess::ProcessData &proc);
  static std::optional<FormatOptions>
  loadFromProcess(const FFmpegProcess::ProcessData &proc,
                  const QByteArrayView &name);

  QByteArray name() const;
  QByteArray description() const;
  bool supportsDemux() const;
  bool supportsMux() const;

  /*
  @brief Strong ordering comparator
  */
  bool operator<(const FormatOptions &rhs) const;

private:
  QByteArray m_name, m_desc;
  bool m_demuxer;
  bool m_muxer;
};

// Bitstream filters are just characterized by name
std::set<QByteArray>
loadAllBitstreamFiltersFromProcess(const FFmpegProcess::ProcessData &proc);

class PixelFormatOptions {
  Q_GADGET

  friend class PixelFormatOptionsPrivate;

public:
  QByteArray name() const;
  bool supportedInputForConversion() const;
  bool supportedOutputForConversion() const;
  bool hardwareAccelerated() const;
  bool paletted() const;
  bool bitstreamFormat() const;
  QList<int> bitDepths() const;

private:
  QByteArray m_name;
  int m_supportedInput : 1;
  int m_supportedOutput : 1;
  int m_hwAccel : 1;
  int m_paletted : 1;
  int m_bitstream : 1;
  int m_nbComponents;
  int m_bpp;
  QList<int> m_bitDepths;
};

class SampleFormatOptions {
  Q_GADGET

  friend class SampleFormatOptionsPrivate;

public:
  QByteArray name() const;
  int bitDepth() const;

private:
  QByteArray m_name;
  int m_depth;
};
} // namespace VvvfSimulator::Generation::FFmpegProcess::Options