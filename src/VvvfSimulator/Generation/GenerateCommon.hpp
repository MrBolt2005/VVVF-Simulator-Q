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

// Packages
#include <avcpp/codec.h>
// Internal Includes
#include "QtVideoWriter.hpp"
#include "../Yaml/MasconControl/YamlMasconAnalyze.hpp"
#include "../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"

namespace VvvfSimulator::Generation::GenerationCommon
{
	using NAMESPACE_YAMLMASCONCONTROL::YamlMasconAnalyze::YamlMasconDataCompiled;
	using NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;

	typedef QtVideoWriter GenerationVideoWriter;

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
		YamlVvvfSoundData soundData;
		ProgressData progress{};
	};

	class LibAVCodecText
	{
		Q_GADGET

		class PixelFormat
		{
			Q_GADGET
			Q_PROPERTY(int pixFmtNum MEMBER pixFmtNum)
			Q_PROPERTY(QString name MEMBER name)

		public:
			union { AVPixelFormat pixFmt; int pixFmtNum; };
			QString name;
		};

		class SampleFormat
		{
			Q_GADGET
			Q_PROPERTY(int sampleFmtNum MEMBER sampleFmtNum)
			Q_PROPERTY(QString name MEMBER name)

		public:
			union { AVSampleFormat sampleFmt; int sampleFmtNum; };
			QString name;
		};

		Q_PROPERTY(int idNum MEMBER idNum)
		Q_PROPERTY(int typeNum MEMBER typeNum)
		Q_PROPERTY(QString name MEMBER name)
		Q_PROPERTY(QString longName MEMBER longName)
		Q_PROPERTY(int isDecoder MEMBER isDecoder)
		Q_PROPERTY(int props MEMBER props)
		Q_PROPERTY(QStringList mimeTypes MEMBER mimeTypes)
		Q_PROPERTY(QList<double> supportedFramerates MEMBER supportedFramerates)
		Q_PROPERTY(QList<PixelFormat> supportedPixelFormats MEMBER supportedPixelFormats)
		Q_PROPERTY(QList<int> supportedSamplerates MEMBER supportedSamplerates)
		Q_PROPERTY(QList<SampleFormat> supportedSampleFormats MEMBER supportedSampleFormats)

	public:
		union { AVCodecID id; int idNum; };
		union { AVMediaType type; int typeNum; };
		QString name;
		QString longName;
		int isDecoder;
		int props;
		QStringList mimeTypes;
		QList<double> supportedFramerates;
		QList<PixelFormat> supportedPixelFormats;
		QList<int> supportedSamplerates;
		QList<SampleFormat> supportedSampleFormats;

		LibAVCodecText(const av::Codec &codec);
		~LibAVCodecText() = default;

		constexpr auto sizeOf() const noexcept { return sizeof(*this); }
	};
}