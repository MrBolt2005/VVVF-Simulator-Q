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

// GenerateControlOriginal.hpp
// Version 1.9.1.1

// Internal
#include "../../../GUI/Util/BitmapViewer.qml.hpp"
#include "../../GenerateCommon.hpp"
#include "../../../Vvvf/Struct.hpp"
// Packages
#include <QDir>
#include <QFont>
#include <QImage>

namespace VvvfSimulator::Generation::Video::ControlInfo::GenerateControlOriginal
{
	using NAMESPACE_VVVF::Struct::VvvfValues;

	QImage getImage (
		const VvvfValues &control,
		bool finalShow,
		int width,
		int height,
		const QFont &titleFnt,
		const QFont &valFnt,
		const QFont &valMiniFnt,
		bool darkMode = false
	);

	void exportVideo(
		GenerationCommon::GenerationBasicParameter &generationBasicParameter,
		const QDir &outputPath,
		double FPS,
		int width,
		int height,
		const QFont &titleFnt,
		const QFont &valFnt,
		const QFont &valMiniFnt,
		bool darkMode = false
	);
}