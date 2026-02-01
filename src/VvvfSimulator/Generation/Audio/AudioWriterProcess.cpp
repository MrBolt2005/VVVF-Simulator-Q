#include "AudioWriterProcess.hpp"

#define LOCK_PROLOGUE std::lock_guard locker(*(m_lock.data.load()));

namespace VvvfSimulator::Generation::Audio {
using namespace FFmpegProcess;
using namespace FFmpegProcess::Options;

void AudioWriter::writeSamples(const std::span<const char> &data) {
  LOCK_PROLOGUE
  decltype(m_err) error;

  if (!isOpen()) {
    error.sourceText =
        QByteArrayLiteral("Failed to write samples: the writer is not open.");
    m_err = std::move(error);
    emit errorOccurred();
    return;
  }

  // Check if byte count matches the bytes-per-sample-channel mark desired
  std::size_t samples = data.size() / (m_ch * (m_sampleFmt.bitDepth() >> 3));
  std::size_t sampRem = data.size() % (m_ch * (m_sampleFmt.bitDepth() >> 3));
  if (sampRem) {
    error.sourceText = QByteArrayLiteral(
        "Failed to write samples: data byte count must be a multiple of the "
        "writer's specified bit depth and channel count.");
    m_err = std::move(error);
    emit errorOccurred();
    return;
  }

  Q_ASSERT(data.size() <= std::numeric_limits<qint64>().max());
  auto written = m_proc.write(data.data(), data.size());
  if (written != data.size()) {
    error.sourceText =
        QByteArrayLiteral("Failed to write samples: not all sample data bytes "
                          "were written successfully.");
    m_err = std::move(error);
    emit errorOccurred();
    return;
  }

  emit samplesWritten(samples, written);
}
} // namespace VvvfSimulator::Generation::Audio