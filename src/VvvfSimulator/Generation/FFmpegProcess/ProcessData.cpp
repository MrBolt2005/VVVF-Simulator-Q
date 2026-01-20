#include "ProcessData.hpp"
// Packages
#include <QObject>

namespace VvvfSimulator::Generation::FFmpegProcess {
    namespace {
        QByteArray readOneShot(const std::filesystem::path &path, const QStringList &arguments)
        {
            QProcess proc;

            proc.setProgram(QString::fromStdU16String(path.u16string()));
            proc.setArguments(arguments);

            volatile bool started = false;
            volatile int errCode = -1;

            auto hCheckStart = QObject::connect(&proc, &QProcess::started, [&started]() {
                started = true;
            });
            auto hCheckFail = QObject::connect(&proc, &QProcess::errorOccurred, [&errCode](QProcess::ProcessError error) {
                errCode = error;
            });

            proc.start();

            while (true) {
                if (started) {
                    bool finished = proc.waitForFinished();
                    if (!finished) {
                        if (errCode == -1)
                            errCode = proc.error();
                        goto fail;
                    }
                    return proc.readAllStandardOutput();
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

    void ProcessData::getFFmpegFeatures(const std::filesystem::path &path, QByteArrayList &enabled, QByteArrayList &disabled)
    {
        auto res = readOneShot(path, {QStringLiteral("-version")});
        
        QByteArrayList words = res.split(' '), retVal;
        for (auto &word : words) {
            static constexpr QByteArrayView keyEnable("--enable-");
            static constexpr QByteArrayView keyDisable("--disable-");

            word = word.trimmed();            
            if (word.startsWith(keyEnable)) {
                QByteArrayView newWord(word.constData() + keyEnable.size());
                Q_ASSERT(newWord.size() == word.size() - keyEnable.size());
                enabled.emplace_back(newWord);
            }
            else if (word.startsWith(keyDisable)) {
                QByteArrayView newWord(word.constData() + keyDisable.size());
                Q_ASSERT(newWord.size() == word.size() - keyDisable.size());
                disabled.emplace_back(newWord);
            }
        }

        // return Outcome::success();
    }

    QByteArray ProcessData::getFFmpegVersion(const std::filesystem::path &path)
    {
        return readOneShot(path, {QStringLiteral("-version")});
    }

    namespace {
    QByteArray getFFmpegHelpCommon(const std::filesystem::path &path,
                                   const QStringView &name, QString kind,
                                   ProcessData::HelpLength length) {
      QStringList args{QStringLiteral("-hide_banner"), QStringLiteral("-h")};
      if (length == ProcessData::HelpLength::Long)
        args.emplace_back(QStringLiteral("long"));
      else if (length == ProcessData::HelpLength::Full)
        args.emplace_back(QStringLiteral("full"));
      args.emplace_back(std::move(kind.append(name)));
      return readOneShot(path, args);
    }
    }
    
    QByteArray
    ProcessData::getFFmpegHelpEncoder(const std::filesystem::path &path,
                                      const QStringView &name,
                                      HelpLength length) {
      return getFFmpegHelpCommon(path, name, QStringLiteral("encoder="), length);
    }

    QByteArray ProcessData::getFFmpegHelpFormat(const std::filesystem::path &path, const QStringView &name, HelpLength length)
    {
        return QByteArray();
    }
}