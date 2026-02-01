#include "FFmpegLoader.hpp"

namespace VvvfSimulator::Library::FFmpegLoader {
namespace {
Util::String::TranslatableFmtString s_error;
bool s_lastHadError;
FunctionMap s_funcCache;
QLibrary s_library;

// TODO: Populate this
constexpr auto s_minVer = "";
} // namespace

QPair<QByteArray, bool> load(const QString &fileName, const QVariant *version) {
  if (s_library.isLoaded())
    return {QByteArray(), true};
  static const QPair<QByteArray, bool> failVal = {QByteArray(), false};
  Util::String::TranslatableFmtString error;

  if (!version)
    s_library.setFileName(fileName);
  if (version->canConvert(QVariant::Int))
    s_library.setFileNameAndVersion(fileName, version->toInt());
  else if (version->canConvert(QVariant::String))
    s_library.setFileNameAndVersion(fileName, version->toString());
  else
    s_library.setFileName(fileName);

  s_library.load();
  if (s_library.isLoaded()) {
    error.sourceText = "QLibrary: %1";
    error.args = {{s_library.errorString()}};
    
    s_error = std::move(error);
    return failVal;
  }

  // Check if it's an FFmpeg library by trying to load the version getter
}
} // namespace VvvfSimulator::Library::FFmpegLoader