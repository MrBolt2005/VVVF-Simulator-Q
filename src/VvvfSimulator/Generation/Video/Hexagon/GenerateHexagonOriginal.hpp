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

// GenerateHexagonOriginal.hpp
// Version 1.9.1.1

// Internal
#include "../../GenerateCommon.hpp"
#include "../../../Vvvf/InternalMath.hpp"
#include "../../../Vvvf/Struct.hpp"
#include "../../../Yaml/VvvfSound/YamlVvvfAnalyze.hpp"
// Packages
#include <QDir>
#include <QImage>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QVector>

namespace VvvfSimulator::Generation::Video::ControlInfo::GenerateHexagonOriginal
{
	using NAMESPACE_VVVF::Struct::VvvfValues;
	using NAMESPACE_VVVF::Struct::WaveValues;
	using NAMESPACE_YAMLVVVFSOUND::YamlVvvfSoundData;
	
	/// <summary>
	/// Gets image of Voltage Vector Hexagon
	/// </summary>
	/// <param name="Control">Make sure you do clone.</param>
	/// <param name="Sound"></param>
	/// <param name="Width"></param>
	/// <param name="Height"></param>
	/// <param name="Delta"></param>
	/// <param name="Thickness"></param>
	/// <param name="ZeroVectorCircle"></param>
	/// <param name="PreciseDelta"></param>
	/// <returns></returns>
	inline QImage getImage(
		const VvvfValues &control,
		const YamlVvvfSoundData &sound,
		const QSize &size,
		int delta,
		qreal thickness,
		bool zeroVectorCircle,
		bool preciseDelta,
		bool darkMode = false
	)
	{
		const auto PWM_Array = GenerateBasic::WaveForm::getUVWCycle(control, sound, 0.0, delta, preciseDelta);

		return getImage(PWM_Array, control.controlFrequency, size, thickness, zeroVectorCircle, darkMode);
	}

	QImage getImage(
		const QVector<WaveValues> &UVW,
		double controlFrequency,
		const QSize &size,
		int thickness,
		bool zeroVectorCircle,
		bool darkMode = false
	);

	void exportVideo(
		GenerationCommon::GenerationBasicParameter &generationBasicParameter,
		const QDir &fileName,
		bool circle,
		double fps = 60.0,
		const QSize &size = QSize(1000, 1000),
		AVCodecID codec = AV_CODEC_ID_H264
	);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="fileName"> Path for png file </param>
	/// <param name="sound_data">SOUND DATA</param>
	/// <param name="circle">Setting whether see zero vector circle or not</param>
	/// <param name="d">Frequency you want to see</param>
	void exportImage(
		const QDir &fileName,
		const YamlVvvfSoundData &soundData,
		bool drawZeroVectorCircle,
		double d,
		const QSize &size = QSize(1000, 1000),
		const char *format = nullptr
	);
}