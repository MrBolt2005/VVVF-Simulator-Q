#include "LoadMidi.qml.hpp"

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

// LoadMidi.qml.cpp
// Version 1.9.1.1

// Packages
#include <QFileDialog>
#include <QDebug>

namespace VvvfSimulator::GUI::Mascon
{
	LoadMidi::LoadMidi(const QVariantMap *const params, QObject *parent)
		: WindowPropertyBase(params, parent)
	{
	}

	void LoadMidi::setTrack(int track)
	{
		if (m_loadConfiguration.track != track) {
			m_loadConfiguration.track = track;
			emit trackChanged(m_loadConfiguration.track);
			emit midiDataChanged(m_loadConfiguration);
		}
	}

	void LoadMidi::setPriority(int priority)
	{
		if (m_loadConfiguration.priority != priority) {
			m_loadConfiguration.priority = priority;
			emit priorityChanged(m_loadConfiguration.priority);
			emit midiDataChanged(m_loadConfiguration);
		}
	}

	void LoadMidi::setDivision(double division)
	{
		if (!qFuzzyCompare(m_loadConfiguration.division, division)) {
			m_loadConfiguration.division = division;
			emit divisionChanged(m_loadConfiguration.division);
			emit midiDataChanged(m_loadConfiguration);
		}
	}

	void LoadMidi::setPath(const QString &path)
	{
		if (m_loadConfiguration.path != path) {
			m_loadConfiguration.path = path;
			emit pathChanged(m_loadConfiguration.path);
			emit midiDataChanged(m_loadConfiguration);
		}
	}

	void LoadMidi::setMidiData(const MidiLoadData &data)
	{
		bool wasDataChanged = false;
		if (m_loadConfiguration.track != data.track)
		{
			m_loadConfiguration.track = data.track;
			emit trackChanged(m_loadConfiguration.track);
			wasDataChanged = true;
		}
		if (m_loadConfiguration.priority != data.priority)
		{
			m_loadConfiguration.priority = data.priority;
			emit priorityChanged(m_loadConfiguration.priority);
			wasDataChanged = true;
		}
		if (!qFuzzyCompare(m_loadConfiguration.division, data.division))
		{
			m_loadConfiguration.division = data.division;
			emit divisionChanged(m_loadConfiguration.division);
			wasDataChanged = true;
		}
		if (m_loadConfiguration.path != data.path)
		{
			m_loadConfiguration.path = data.path;
			emit pathChanged(m_loadConfiguration.path);
			wasDataChanged = true;
		}
		if (wasDataChanged) emit midiDataChanged(m_loadConfiguration);
	}

	void LoadMidi::selectFile()
	{
		QString fileName = QFileDialog::getOpenFileName(nullptr, tr("Select MIDI File"), QString(), tr("MIDI Files (*.mid *.midi)"));
		if (!fileName.isEmpty())
		{
			setPath(fileName);
			//qDebug() << "Selected file:" << fileName;
		}
	}
}