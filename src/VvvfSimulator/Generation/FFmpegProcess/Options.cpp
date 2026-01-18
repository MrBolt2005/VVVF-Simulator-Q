#include "Options.hpp"
// Standard Library
#include <memory>

#define STRONG_COMPARE_RETURN(attrib) \
if (##attrib < rhs.##attrib) return true;

namespace VvvfSimulator::Generation::FFmpegProcess::Options {
    namespace {
        Outcome::ErrnoResult<std::unique_ptr<QProcess>> tryOneShot(const std::filesystem::path &path, const QStringList &arguments)
        {
            auto proc = std::make_unique<QProcess>();

            proc->setProgram(QString::fromStdU16String(path.u16string()));
            proc->setArguments(arguments);

            volatile bool started = false;
            volatile int errCode = -1;

            auto hCheckStart = QObject::connect(proc.get(), &QProcess::started, [&started]() {
                started = true;
            });
            auto hCheckFail = QObject::connect(proc.get(), &QProcess::errorOccurred, [&errCode](QProcess::ProcessError error) {
                errCode = error;
            });

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
            return errCode;
        }
    } // anonymous namespace
    
    Outcome::ErrnoResult<std::set<EncoderOptions>> EncoderOptions::loadAllFromProcess(const FFmpegProcess::ProcessData &proc)
    {
        auto res = tryOneShot(proc.path(), {QStringLiteral("-hide_banner"), QStringLiteral("-encoders")});
        if (res.has_error())
            return res.error();
        // Q_ASSERT(!res.has_lost_consistency());
        // Q_ASSERT(res.has_value());
        
        // Seek separator line, which contains only the '-' character
        auto &resProc = res.value();
        QByteArray line;
        bool loop;
        do {
            loop = false;

            if (!resProc->canReadLine())
                return -1;
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
                return -2;

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
                return -2;
            }
            options.m_frameLevelMT  = flags[1] == 'F' ? 1 : 0;
            options.m_sliceLevelMT  = flags[2] == 'S' ? 1 : 0;
            options.m_experimental  = flags[3] == 'X' ? 1 : 0;
            options.m_drawHorizBand = flags[4] == 'B' ? 1 : 0;
            options.m_drm1          = flags[5] == 'D' ? 1 : 0;
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

    Outcome::ErrnoResult<std::optional<EncoderOptions>> EncoderOptions::loadFromProcess(const FFmpegProcess::ProcessData &proc, const QByteArrayView &name)
    {
        auto allNames = loadAllFromProcess(proc);
        if (allNames.has_error())
            return allNames.error();
        auto found = std::lower_bound(
            allNames.value().cbegin(),
            allNames.value().cend(),
            ([](const auto &l, const auto &r) { return l.m_name == r.m_name; })
        );
        if (found == allNames.value().cend())
            return std::nullopt;
        if (found->m_name != name)
            return std::nullopt;
        return *found;
    }

    QByteArray EncoderOptions::name() const
    {
        return m_name;
    }

    QByteArray EncoderOptions::description() const
    {
        return m_desc;
    }

    EncoderOptions::EncoderType EncoderOptions::type() const
    {
        return m_type;
    }

    bool EncoderOptions::supportsFrameLevelMultiThreading() const
    {
        return m_frameLevelMT != 0;
    }

    bool EncoderOptions::supportsSliceLevelMultiThreading() const
    {
        return m_sliceLevelMT != 0;
    }

    bool EncoderOptions::experimental() const
    {
        return m_experimental != 0;
    }

    bool EncoderOptions::supportsDrawHorizontalBand() const
    {
        return m_drawHorizBand != 0;
    }

    bool EncoderOptions::supportsDRM1() const
    {
        return m_drm1 != 0;
    }

    bool EncoderOptions::operator<(const EncoderOptions &rhs) const
    {
        STRONG_COMPARE_RETURN(m_name)
        STRONG_COMPARE_RETURN(m_desc)
        STRONG_COMPARE_RETURN(m_type)
        STRONG_COMPARE_RETURN(m_frameLevelMT)
        STRONG_COMPARE_RETURN(m_sliceLevelMT)
        STRONG_COMPARE_RETURN(m_experimental)
        STRONG_COMPARE_RETURN(m_drawHorizBand)
        STRONG_COMPARE_RETURN(m_drm1)
        return false;
    }

    Outcome::ErrnoResult<std::set<FormatOptions>> FormatOptions::loadAllFromProcess(const FFmpegProcess::ProcessData &proc)
    {
        auto res = tryOneShot(proc.path(), {QStringLiteral("-hide_banner"), QStringLiteral("-formats")});
        if (res.has_error())
            return res.error();
        // Q_ASSERT(!res.has_lost_consistency());
        // Q_ASSERT(res.has_value());
        
        // Seek separator line, which contains only the '-' character
        auto &resProc = res.value();
        QByteArray line;
        bool loop;
        do {
            loop = false;

            if (!resProc->canReadLine())
                return -1;
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
                return -2;
            
            options.m_demuxer = flags.contains('D') ? 1 : 0;
            options.m_muxer   = flags.contains('E') ? 1 : 0;
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

    Outcome::ErrnoResult<std::optional<FormatOptions>> FormatOptions::loadFromProcess(const FFmpegProcess::ProcessData &proc, const QByteArrayView &name)
    {
        auto allNames = loadAllFromProcess(proc);
        if (allNames.has_error())
            return allNames.error();
        auto found = std::lower_bound(
            allNames.value().cbegin(),
            allNames.value().cend(),
            ([](const auto &l, const auto &r) { return l.m_name == r.m_name; })
        );
        if (found == allNames.value().cend())
            return std::nullopt;
        if (found->m_name != name)
            return std::nullopt;
        return *found;
    }
}