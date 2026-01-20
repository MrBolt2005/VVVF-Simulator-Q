#include "Options.hpp"
// Standard Library
#include <cstdlib> // for EXIT_SUCCESS
#include <memory>
// Packages
#include <QDebug>

static_assert(EXIT_SUCCESS == 0, "We are on a platform where the EXIT_SUCCESS "
                                 "code isn't 0, investigate!");

// #define STRONG_COMPARE_RETURN(attrib) \
// if (##attrib < rhs.##attrib) return true;

namespace VvvfSimulator::Generation::FFmpegProcess::Options {
namespace {
std::unique_ptr<QProcess> tryOneShot(const std::filesystem::path &path,
                                     const QStringList &arguments,
                                     int &outExitCode) {
  auto proc = std::make_unique<QProcess>();

  proc->setProgram(QString::fromStdU16String(path.u16string()));
  proc->setArguments(arguments);

  volatile bool started = false;
  volatile int errCode = -1;

  auto hCheckStart = QObject::connect(proc.get(), &QProcess::started,
                                      [&started]() { started = true; });
  auto hCheckFail = QObject::connect(
      proc.get(), &QProcess::errorOccurred,
      [&errCode](QProcess::ProcessError error) { errCode = error; });

  while (true) {
    if (started) {
      bool exiting = proc->waitForFinished();
      if (!exiting) {
        if (errCode == -1)
          errCode = proc->error();
        goto fail;
      }
      outExitCode = proc->exitCode();
      return proc;
    }
    if (errCode != -1)
      goto fail;
  };

fail:
  const char *what;
  switch (errCode) {
  case QProcess::FailedToStart:
    what = "The process failed to start.";
    break;
  case QProcess::Crashed:
    what = "The process crashed some time after starting successfully.";
    break;
  case QProcess::Timedout:
    what = "The last process wait timed out.";
    break;
  case QProcess::WriteError:
    what = "An error occurred when attempting to write to the process.";
    break;
  case QProcess::ReadError:
    what = "An error occurred when attempting to read from the process.";
    break;
  default:
    what = "An unknown error occurred.";
  }
  throw std::runtime_error(what);
}
} // anonymous namespace

std::set<EncoderOptions>
EncoderOptions::loadAllFromProcess(const FFmpegProcess::ProcessData &proc) {
  int exitCode;
  auto resProc = tryOneShot(
      proc.path(),
      {QStringLiteral("-hide_banner"), QStringLiteral("-encoders")}, exitCode);

  if (exitCode != EXIT_SUCCESS)
    throw std::runtime_error("The process didn't return success when exiting.");

  // Seek separator line, which contains only the '-' character
  QByteArray line;
  bool loop;
  do {
    loop = false;

    if (!resProc->canReadLine())
      throw std::runtime_error(qPrintable(resProc->errorString().prepend(
          "Can not read line(s) from the process's "
          "standard output: ")));
    line = resProc->readLine().trimmed();
    for (const auto &c : line)
      if (c != '-')
        loop = true;
  } while (loop);

  std::set<EncoderOptions> retVal;

  // Read codec lines
  while (resProc->canReadLine()) {
    EncoderOptions options;

    line = resProc->readLine().trimmed();
    QByteArrayList words = line.split(' ');

    // Filter out multiple sequential spaces
    words.removeAll("");
    // Size check
    if (words.size() < 2)
      throw std::runtime_error("The process's standard output is misformatted: "
                               "incorrect section count.");

    // First word: flag set
    auto &flags = *(words.begin());

    // Length check
    if (flags.size() < 6)
      throw std::runtime_error(
          "The process's standard output is misformatted: failed to "
          "parse encoder option flags.");

    switch (flags[0]) {
    case 'V':
      options.m_type = EncoderType::Video;
      break;
    case 'A':
      options.m_type = EncoderType::Audio;
      break;
    case 'S':
      options.m_type = EncoderType::Subtitle;
      break;
    default:
      throw std::runtime_error(
          "The process's standard output is misformatted: failed to "
          "parse encoder option type.");
    }
    options.m_frameLevelMT = flags[1] == 'F' ? 1 : 0;
    options.m_sliceLevelMT = flags[2] == 'S' ? 1 : 0;
    options.m_experimental = flags[3] == 'X' ? 1 : 0;
    options.m_drawHorizBand = flags[4] == 'B' ? 1 : 0;
    options.m_drm1 = flags[5] == 'D' ? 1 : 0;
    // Skip 1st word
    words.pop_front();

    // 2nd word: name
    options.m_name = *words.begin();
    words.pop_front();

    // Remaining words: description
    options.m_desc = words.join(' ');

    // Send constructed object to return value
    retVal.emplace(std::move(options));
  }

  return retVal;
}

std::optional<EncoderOptions>
EncoderOptions::loadFromProcess(const FFmpegProcess::ProcessData &proc,
                                const QStringView &name) {
  auto allNames = loadAllFromProcess(proc);
  auto found =
      std::lower_bound(allNames.cbegin(), allNames.cend(), name,
                       ([](const EncoderOptions &l, const QStringView &r) {
                         return l.m_name < r;
                       }));
  if (found == allNames.cend())
    return std::nullopt;
  if (found->m_name != name)
    return std::nullopt;
  return *found;
}

QString EncoderOptions::name() const { return m_name; }

QString EncoderOptions::description() const { return m_desc; }

EncoderOptions::EncoderType EncoderOptions::type() const { return m_type; }

bool EncoderOptions::supportsFrameLevelMultiThreading() const {
  return m_frameLevelMT != 0;
}

bool EncoderOptions::supportsSliceLevelMultiThreading() const {
  return m_sliceLevelMT != 0;
}

bool EncoderOptions::experimental() const { return m_experimental != 0; }

bool EncoderOptions::supportsDrawHorizontalBand() const {
  return m_drawHorizBand != 0;
}

bool EncoderOptions::supportsDRM1() const { return m_drm1 != 0; }

bool EncoderOptions::operator<(const EncoderOptions &rhs) const {
  if (m_name < rhs.m_name)
    return true;
  if (m_desc < rhs.m_desc)
    return true;
  if (m_type < rhs.m_type)
    return true;
  if (m_frameLevelMT < rhs.m_frameLevelMT)
    return true;
  if (m_sliceLevelMT < rhs.m_sliceLevelMT)
    return true;
  if (m_experimental < rhs.m_experimental)
    return true;
  if (m_drawHorizBand < rhs.m_drawHorizBand)
    return true;
  return m_drm1 < rhs.m_drm1;
}

std::set<FormatOptions>
FormatOptions::loadAllFromProcess(const FFmpegProcess::ProcessData &proc) {
  int exitCode;
  auto resProc = tryOneShot(
      proc.path(), {QStringLiteral("-hide_banner"), QStringLiteral("-formats")},
      exitCode);

  if (exitCode != EXIT_SUCCESS)
    throw std::runtime_error("The process didn't return success when exiting.");

  // Seek separator line, which contains only the '-' character
  QByteArray line;
  bool loop;
  do {
    loop = false;

    if (!resProc->canReadLine())
      throw std::runtime_error(qPrintable(resProc->errorString().prepend(
          "Can not read line(s) from the process's "
          "standard output: ")));
    line = resProc->readLine().trimmed();
    for (const auto &c : line)
      if (c != '-')
        loop = true;
  } while (loop);

  std::set<FormatOptions> retVal;

  // Read lines
  while (resProc->canReadLine()) {
    FormatOptions options;

    line = resProc->readLine().trimmed();
    QByteArrayList words = line.split(' ');

    // Filter out multiple sequential spaces
    words.removeAll("");
    // Size check
    if (words.size() < 2)
      throw std::runtime_error("The process's standard output is misformatted: "
                               "incorrect section count.");

    // First word: flag set
    auto &flags = *(words.begin());

    // Length check
    if (flags.size() < 2)
      throw std::runtime_error(
          "The process's standard output is misformatted: failed to "
          "parse format option flags.");

    options.m_demuxer = flags.contains('D') ? 1 : 0;
    options.m_muxer = flags.contains('E') ? 1 : 0;
    // Skip 1st word
    words.pop_front();

    // 2nd word: name
    options.m_name = *words.begin();
    words.pop_front();

    // Remaining words: description
    options.m_desc = words.join(' ');

    // Send constructed object to return value
    retVal.emplace(std::move(options));
  }

  return retVal;
}

std::optional<FormatOptions>
FormatOptions::loadFromProcess(const FFmpegProcess::ProcessData &proc,
                               const QStringView &name) {
  auto allNames = loadAllFromProcess(proc);
  auto found =
      std::lower_bound(allNames.cbegin(), allNames.cend(), name,
                       ([](const FormatOptions &l, const QStringView &r) {
                         return l.m_name < r;
                       }));
  if (found == allNames.cend())
    return std::nullopt;
  if (found->m_name != name)
    return std::nullopt;
  return *found;
}

QString FormatOptions::name() const { return m_name; }

QString FormatOptions::description() const { return m_desc; }

bool FormatOptions::supportsDemux() const { return m_demuxer; }

bool FormatOptions::supportsMux() const { return m_muxer; }

bool FormatOptions::operator<(const FormatOptions &rhs) const {
  if (m_name < rhs.m_name)
    return true;
  if (m_desc < rhs.m_desc)
    return true;
  if (m_demuxer < rhs.m_demuxer)
    return true;
  return m_muxer < rhs.m_muxer;
}

std::set<QString>
loadAllBitstreamFiltersFromProcess(const FFmpegProcess::ProcessData &proc) {
  int exitCode;
  auto resProc = tryOneShot(
      proc.path(), {QStringLiteral("-hide_banner"), QStringLiteral("-bsfs")},
      exitCode);

  if (exitCode != EXIT_SUCCESS)
    throw std::runtime_error("The process didn't return success when exiting.");

  // Skip line 1, which just says "Bitstream filters:"
  auto header = resProc->readLine().trimmed();
  if (header != "Bitstream filters:")
    qWarning() << QObject::tr(
                      "Expected process standard output's header line to be "
                      "\"Bitstream filters:\", but instead got \"%1\".")
                      .arg(header);

  std::set<QString> retVal;

  if (!resProc->canReadLine())
    throw std::runtime_error(qPrintable(resProc->errorString().prepend(
        "Can not read line(s) from the process's "
        "standard output: ")));
  while (resProc->canReadLine())
    retVal.emplace(resProc->readLine().trimmed());

  return retVal;
}

std::set<PixelFormatOptions>
PixelFormatOptions::loadAllFromProcess(const FFmpegProcess::ProcessData &proc) {
  int exitCode;
  auto resProc = tryOneShot(
      proc.path(),
      {QStringLiteral("-hide_banner"), QStringLiteral("-pix_fmts")}, exitCode);

  if (exitCode != EXIT_SUCCESS)
    throw std::runtime_error("The process didn't return success when exiting.");

  // Seek separator line, which contains only the '-' character
  QByteArray line;
  bool loop;
  do {
    loop = false;

    if (!resProc->canReadLine())
      throw std::runtime_error(qPrintable(resProc->errorString().prepend(
          "Can not read line(s) from the process's "
          "standard output: ")));
    line = resProc->readLine().trimmed();
    for (const auto &c : line)
      if (c != '-')
        loop = true;
  } while (loop);

  std::set<PixelFormatOptions> retVal;

  // Read lines
  while (resProc->canReadLine()) {
    PixelFormatOptions options;

    line = resProc->readLine().trimmed();
    QByteArrayList words = line.split(' ');

    // Filter out multiple sequential spaces
    words.removeAll("");
    // Size check
    if (words.size() < 5)
      throw std::runtime_error("The process's standard output is misformatted: "
                               "incorrect section count.");

    // First word: flag set
    auto &flags = *(words.begin());

    // Length check
    if (flags.size() < 5)
      throw std::runtime_error(
          "The process's standard output is misformatted: failed to "
          "parse pixel format option flags.");

    options.m_supportedInput = flags.contains('I') ? 1 : 0;
    options.m_supportedOutput = flags.contains('O') ? 1 : 0;
    options.m_hwAccel = flags.contains('H') ? 1 : 0;
    options.m_paletted = flags.contains('P') ? 1 : 0;
    options.m_bitstream = flags.contains('B') ? 1 : 0;
    // Skip 1st word
    words.pop_front();

    // 2nd word: name
    options.m_name = *words.begin();
    words.pop_front();

    // 3rd word: NB_COMPONENTS
    bool ok;
    options.m_nbComponents = words.begin()->toInt(&ok);
    if (!ok)
      throw std::runtime_error(
          "The process's standard output is misformatted: failed to "
          "parse pixel format option components count.");
    words.pop_front();

    // 4th word: bits per pixel
    options.m_bpp = words.begin()->toInt(&ok);
    if (!ok)
      throw std::runtime_error(
          "The process's standard output is misformatted: failed to "
          "parse pixel format option bits-per-pixel count.");
    words.pop_front();

    // 5th word: bit depths
    QByteArrayList bds = words.begin()->split('-');
    options.m_bitDepths.reserve(bds.size());
    for (const auto &sub : bds) {
      int bd = words.begin()->toInt(&ok);
      if (!ok)
        throw std::runtime_error(
            "The process's standard output is misformatted: failed to "
            "parse pixel format option bits-per-pixel count.");
      options.m_bitDepths.emplace_back(bd);
    }
  }

  return retVal;
}

std::optional<PixelFormatOptions>
PixelFormatOptions::loadFromProcess(const FFmpegProcess::ProcessData &proc,
                                    const QStringView &name) {
  auto allNames = loadAllFromProcess(proc);
  auto found =
      std::lower_bound(allNames.cbegin(), allNames.cend(), name,
                       ([](const PixelFormatOptions &l, const QStringView &r) {
                         return l.m_name < r;
                       }));
  if (found == allNames.cend())
    return std::nullopt;
  if (found->m_name != name)
    return std::nullopt;
  return *found;
}

QString PixelFormatOptions::name() const { return m_name; }

bool PixelFormatOptions::supportedInputForConversion() const {
  return m_supportedInput != 0;
}

bool PixelFormatOptions::supportedOutputForConversion() const {
  return m_supportedOutput != 0;
}

bool PixelFormatOptions::hardwareAccelerated() const { return m_hwAccel != 0; }

bool PixelFormatOptions::paletted() const { return m_paletted != 0; }

bool PixelFormatOptions::bitstreamFormat() const { return m_bitstream != 0; }

QList<int> PixelFormatOptions::bitDepths() const { return m_bitDepths; }

bool PixelFormatOptions::operator<(const PixelFormatOptions &rhs) const {
  if (m_name < rhs.m_name)
    return true;
  if (m_supportedInput < rhs.m_supportedInput)
    return true;
  if (m_supportedOutput < rhs.m_supportedOutput)
    return true;
  if (m_hwAccel < rhs.m_hwAccel)
    return true;
  if (m_paletted < rhs.m_paletted)
    return true;
  if (m_bitstream < rhs.m_bitstream)
    return true;
  if (m_nbComponents < rhs.m_nbComponents)
    return true;
  if (m_bpp < rhs.m_bpp)
    return true;
  return m_bitDepths < rhs.m_bitDepths;
}

std::set<SampleFormatOptions> SampleFormatOptions::loadAllFromProcess(
    const FFmpegProcess::ProcessData &proc) {
  int exitCode;
  auto resProc = tryOneShot(
      proc.path(),
      {QStringLiteral("-hide_banner"), QStringLiteral("-sample_fmts")},
      exitCode);

  if (exitCode != EXIT_SUCCESS)
    throw std::runtime_error("The process didn't return success when exiting.");

  // Seek header: "name depth"
  QByteArray line;
  QByteArrayList words;

  if (!resProc->canReadLine())
    throw std::runtime_error(qPrintable(resProc->errorString().prepend(
        "Can not read line(s) from the process's "
        "standard output: ")));
  line = resProc->readLine().trimmed();

  words = line.split(' ');

  // Filter out multiple sequential spaces
  words.removeAll("");
  // Size check
  if (words.size() < 2)
    throw std::runtime_error("The process's standard output is misformatted: "
                             "incorrect section count.");

  auto &w1 = words[0], &w2 = words[1];
  if (w1 != "name" || w2 != "depth")
    qWarning()
        << QObject::tr(
               "Expected process standard output's header line to be { "
               "\"name\", \"depth\" }, but instead got { \"%1\", \"%2\" }.")
               .arg(w1)
               .arg(w2);

  std::set<SampleFormatOptions> retVal;

  // Read lines
  while (resProc->canReadLine()) {
    SampleFormatOptions options;

    line = resProc->readLine().trimmed();
    QByteArrayList words = line.split(' ');

    // Filter out multiple sequential spaces
    words.removeAll("");
    // Size check
    if (words.size() < 2)
      throw std::runtime_error("The process's standard output is misformatted: "
                               "incorrect section count.");

    options.m_name = words[0];

    bool ok;
    options.m_depth = words[1].toInt(&ok);
    if (!ok)
      throw std::runtime_error(
          "The process's standard output is misformatted: failed to "
          "parse sample format option bit depth.");

    retVal.emplace(options);
  }

  return retVal;
}

bool SampleFormatOptions::operator<(const SampleFormatOptions &rhs) const {
  if (m_name < rhs.m_name)
    return true;
  return m_depth < rhs.m_depth;
}
} // namespace VvvfSimulator::Generation::FFmpegProcess::Options