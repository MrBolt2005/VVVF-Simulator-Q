#pragma once

/*
	 Copyright © 2025 VvvfGeeks, VVVF Systems
	 
	 Licensed under the Apache License, Version 2.0 (the "License");
	 you may not use this file except in compliance with the License.
	 You may obtain a copy of the License at

			 http://www.apache.org/licenses/LICENSE-2.0

	 Unless required by applicable law or agreed to in writing, software
	 distributed under the License is distributed on an "AS IS" BASIS,
	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	 See the License for the specific language governing permissions and
	 limitations under the License.
*/

// LoadMidi.qml.hpp
// Version 1.9.1.1

// Packages
#include <QObject>
#include <QString>
#include <QDir>
// Internal
#include "../WindowPropertyBase.hpp"

namespace VvvfSimulator::GUI::Mascon
{
	class LoadMidi : public WindowPropertyBase
	{
		Q_OBJECT
		Q_PROPERTY(int track READ track WRITE setTrack NOTIFY trackChanged)
		Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged)
		Q_PROPERTY(double division READ division WRITE setDivision NOTIFY divisionChanged)
		Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
		Q_PROPERTY(MidiLoadData midiData READ midiData WRITE setMidiData NOTIFY midiDataChanged)

	public:
		class MidiLoadData
		{
			Q_GADGET
			Q_PROPERTY(int track MEMBER track)
			Q_PROPERTY(int priority MEMBER priority)
			Q_PROPERTY(double division MEMBER division)
			Q_PROPERTY(QString path MEMBER path)

		public:
			int track = 1;
			int priority = 1;
			double division = 1.0;
			QString path;

			constexpr MidiLoadData() noexcept = default;

			MidiLoadData(int Track, int Priority = 1, double Division = 1.0, const QString &Path = QString()) noexcept :
				track(Track),
				priority(Priority),
				division(Division),
				path(Path)
			{}

			MidiLoadData(int Track, int Priority = 1, double Division = 1.0, QString &&Path = QString()) noexcept :
				track(Track),
				priority(Priority),
				division(Division),
				path(std::move(Path))
			{}
		};

	private:
		MidiLoadData m_loadConfiguration;
				
	public:
		explicit LoadMidi(const QVariantMap *const params = nullptr, QObject *parent = nullptr);

		// Getters
		constexpr int          track()    const noexcept { return m_loadConfiguration.track; }
		constexpr int          priority() const noexcept { return m_loadConfiguration.priority; }
		constexpr double       division() const noexcept { return m_loadConfiguration.division; }
		          QString      path()     const noexcept { return m_loadConfiguration.path; }
		          MidiLoadData midiData() const noexcept { return m_loadConfiguration; }

		// Setters
		void setTrack(int track);
		void setPriority(int priority);
		void setDivision(double division);
		void setPath(const QString &path);
		void setMidiData(const MidiLoadData &data);

	signals:
		void trackChanged(int track);
		void priorityChanged(int priority);
		void divisionChanged(double division);
		void pathChanged(const QString &path);
		void midiDataChanged(const MidiLoadData &data);

	public slots:
		void selectFile();
	};
}