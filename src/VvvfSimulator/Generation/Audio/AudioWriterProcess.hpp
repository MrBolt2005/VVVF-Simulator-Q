#pragma once

// Copyright © 2026 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-or-later
//
// Generation/Audio/AudioWriter.hpp
// v1.10.0.0

// Standard Library
#include <cinttypes>
#include <filesystem>
#include <utility>
// Packages
#include <libavformat/avformat.h>
#include <QByteArrayView>
#include <QMap>
#include <QObject>
#include <QString>
// Internal
#include "Options.hpp"
#include "../../Util/String.hpp"
#include "../../Util/Thread.hpp"

namespace VvvfSimulator::Generation::Audio {
    class AudioWriter : public QObject {
        Q_OBJECT

        // Should not be implemented
        explicit AudioWriter(QObject *parent = nullptr);

    public:
        AudioWriter(const std::filesystem::path &fileName, int64_t bitRate,
                  const std::pair<int64_t, int64_t> &bitRateRange,
                  const QByteArrayView &codec, int channels,
                  AVSampleFormat sampleFormat, QObject *parent = nullptr);
        ~AudioWriter() override;

        // Getters
		int64_t                             bitRate()        const;
		std::pair<int64_t, int64_t>         bitRateRange()   const;
		AVChannelLayout                     channelLayout()  const;
		int                                 channels()       const;
		QByteArray                          codec()          const;
		QString                             errorString()    const;
		Util::String::TranslatableFmtString errorStringRaw() const;
		std::filesystem::path               fileName()       const;
		int                                 flags()          const;
		int                                 flags2()         const;
		bool                                isOpen()         const;
		QMap<QByteArray, QByteArray>        option()         const;
		AVSampleFormat                      sampleFormat()   const;
		int                                 sampleRate()     const;
		int                                 strict()         const;

    private:
        Util::Thread::MutexAPtrWrapper m_lock;
    };
}