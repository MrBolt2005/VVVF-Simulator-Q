#include "Options.hpp"
// Standard Library
#include <memory>

// #define STRONG_COMPARE_RETURN(attrib) \
// if (##attrib < rhs.##attrib) return true;

namespace VvvfSimulator::Generation::FFmpegProcess::Options {
namespace {
std::unique_ptr<QProcess> tryOneShot(const std::filesystem::path &path,
                                     const QStringList &arguments) {
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
      bool finished = proc->waitForFinished();
      if (!finished) {
        if (errCode == -1)
          errCode = proc->error();
        goto fail;
      }
      return proc;
    }
    if (errCode != -1)
      goto fail;
  };

fail:
  std::string_view what;
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
  throw std::runtime_error(what.data());
}
} // anonymous namespace

std::set<EncoderOptions>
EncoderOptions::loadAllFromProcess(const FFmpegProcess::ProcessData &proc) {
  auto resProc = tryOneShot(proc.path(), {QStringLiteral("-hide_banner"),
                                          QStringLiteral("-encoders")});

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

    line = resProc->readLine();
    QByteArrayList words = line.split(' ');

    // Filter out multiple sequential spaces
    words.removeAll("");

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
    options.m_name = std::move(*words.begin());
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
                                const QByteArrayView &name) {
  auto allNames = loadAllFromProcess(proc);
  auto found =
      std::lower_bound(allNames.cbegin(), allNames.cend(), name,
                       ([](const EncoderOptions &l, const QByteArrayView &r) {
                         return l.m_name < r;
                       }));
  if (found == allNames.cend())
    return std::nullopt;
  if (found->m_name != name)
    return std::nullopt;
  return *found;
}

QByteArray EncoderOptions::name() const { return m_name; }

QByteArray EncoderOptions::description() const { return m_desc; }

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
  if (m_drm1 < rhs.m_drm1)
    return true;
  return false;
}

std::set<FormatOptions>
FormatOptions::loadAllFromProcess(const FFmpegProcess::ProcessData &proc) {
  auto resProc = tryOneShot(proc.path(), {QStringLiteral("-hide_banner"),
                                          QStringLiteral("-formats")});

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

  // Read codec lines
  while (resProc->canReadLine()) {
    FormatOptions options;

    line = resProc->readLine();
    QByteArrayList words = line.split(' ');

    // Filter out multiple sequential spaces
    words.removeAll("");

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
    options.m_name = std::move(*words.begin());
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
                               const QByteArrayView &name) {
  auto allNames = loadAllFromProcess(proc);
  auto found =
      std::lower_bound(allNames.cbegin(), allNames.cend(), name,
                       ([](const FormatOptions &l, const QByteArrayView &r) {
                         return l.m_name < r;
                       }));
  if (found == allNames.cend())
    return std::nullopt;
  if (found->m_name != name)
    return std::nullopt;
  return *found;
}

QByteArray FormatOptions::name() const { return m_name; }

QByteArray FormatOptions::description() const { return m_desc; }

bool FormatOptions::supportsDemux() const { return m_demuxer; }

bool FormatOptions::supportsMux() const { return m_muxer; }

bool FormatOptions::operator<(const FormatOptions &rhs) const {
  if (m_name < rhs.m_name)
    return true;
  if (m_desc < rhs.m_desc)
    return true;
  if (m_demuxer < rhs.m_demuxer)
    return true;
  if (m_muxer < rhs.m_muxer)
    return true;
  return false;
}
} // namespace VvvfSimulator::Generation::FFmpegProcess::Options