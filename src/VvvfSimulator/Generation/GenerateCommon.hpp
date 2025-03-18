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

// GenerateCommon.hpp
// Version 1.9.1.1

// Internal Includes
#include "QtVideoWriter.hpp"
#include "../Yaml/MasconControl/YamlMasconAnalyze.hpp"
#include "../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"

namespace VvvfSimulator::Generation::GenerationCommon
{
	using NAMESPACE_YAMLMASCONCONTROL::YamlMasconAnalyze::YamlMasconDataCompiled;
	using NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;

	class GenerationVideoWriter : public QtVideoWriter
	{
		Q_OBJECT

		public:
		GenerationVideoWriter(
			const QDir& filename,
			const int width,
			const int height,
			const double fps,
			const AVCodecID codecID,
			const av::PixelFormat pixelFormat = av::PixelFormat(bestCompatiblePixelFormat),
			bool openOnCreation = false,
			QObject* parent = nullptr) :
		QtVideoWriter(filename, width, height, fps, codecID, pixelFormat, openOnCreation, parent) {}

	public slots:
		void addEmptyFrames(size_t numFrames, bool darkMode = false);
		void addImageFrames(const QImage& image, size_t numFrames);
	};

	struct GenerationBasicParameter
	{
		struct ProgressData
		{
			double progress = 1.0, total = 1.0;
			bool cancel = false;

			constexpr double relativeProgress(bool percent = true) const
			{
				double ret = progress / total;
				return percent ? ret * 100.0 : ret;
			}
		};
		
		YamlMasconDataCompiled masconData{};
		YamlVvvfSoundData soundData{};
		ProgressData progress{};
	}

}