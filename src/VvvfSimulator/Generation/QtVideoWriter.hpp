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

// QtVideoWriter.hpp
// Version 1.9.1.1

// Standard Library Includes
#include <cinttypes>
//#include <expected>
#include <memory>
#include <optional>
//#include <ostream>
#include <string>
// Internal Includes
//#include "../Exception.hpp"
// Package Includes
#include <avcpp/av.h>
#include <avcpp/codec.h>
#include <avcpp/codeccontext.h>
#include <avcpp/format.h>
#include <avcpp/formatcontext.h>
#include <avcpp/frame.h>
#include <avcpp/packet.h>
//#include <avcpp/util.h>
#include <QDir>
#include <QObject>
#include <QImage>

namespace VvvfSimulator::Generation
{
	class QtVideoWriter : public QObject
	{
		Q_OBJECT
		using VideoWriterCodecContextType = av::VideoCodecContext<av::VideoEncoderContext, av::Direction::Encoding>;

		// Report: Trying to reorder members according to creation order

		QDir m_filename; // size : 8 bytes
		int m_width, m_height; // size : 2*4 bytes
		double m_FPS; // size : 8 bytes
		AVCodecID m_codecID; // size : 4 bytes
		av::PixelFormat m_pixelFormat; // size : 4 bytes
		std::optional<av::FormatContext> m_formatContext;
		std::optional<VideoWriterCodecContextType> m_codecContext;
		std::optional<av::Stream> m_stream;
		std::optional<av::Packet> m_packet;
		//av::VideoFrame m_frame;
		int64_t m_pts; // size : 8 bytes
		bool m_isOpen; // size : 1 byte
	public:
		static constexpr AVPixelFormat bestCompatiblePixelFormat = AV_PIX_FMT_RGBAF32;
		static constexpr QImage::Format bestCompatibleImageFormat = QImage::Format_RGBA32FPx4;
		static constexpr QImage::Format bestCompatiblePremultipliedImageFormat = QImage::Format_RGBA32FPx4_Premultiplied;
		//size_t m_s = sizeof(*this);
	
		QtVideoWriter(
			const QDir& filename,
			const int width,
			const int height,
			const double fps,
			const AVCodecID codecID,
			const av::PixelFormat pixelFormat = av::PixelFormat(bestCompatiblePixelFormat),
			bool openOnCreation = false,
			QObject* parent = nullptr);
		~QtVideoWriter() override;

		constexpr const QDir& filename() const { return m_filename; }
		constexpr int width() const noexcept { return m_width; }
		constexpr int height() const noexcept { return m_height; }
		constexpr double FPS() const noexcept { return m_FPS; }
		constexpr av::PixelFormat pixelFormat() const noexcept { return m_pixelFormat; }
		constexpr int64_t pts() const noexcept { return m_pts; }
		constexpr AVCodecID codecID() const noexcept { return m_codecID; }
		constexpr bool isOpen() const noexcept { return m_isOpen; }

		constexpr static AVPixelFormat getPixelFormat(QImage::Format format) noexcept
		{
			switch (format)
			{
			case QImage::Format_Mono:
				return AV_PIX_FMT_MONOBLACK;
			case QImage::Format_MonoLSB:
				return AV_PIX_FMT_MONOWHITE;
			case QImage::Format_Indexed8:
				return AV_PIX_FMT_PAL8;
			case QImage::Format_RGB32:
				return AV_PIX_FMT_RGB32;
			case QImage::Format_ARGB32:
			case QImage::Format_ARGB32_Premultiplied:
				return AV_PIX_FMT_ARGB;
			case QImage::Format_RGB16:
				return AV_PIX_FMT_RGB565;
			case QImage::Format_RGB555:
				return AV_PIX_FMT_RGB555;
			case QImage::Format_RGB888:
				return AV_PIX_FMT_RGB24;
			case QImage::Format_RGB444:
				return AV_PIX_FMT_RGB444;
			case QImage::Format_RGBX8888:
				return AV_PIX_FMT_RGB0;
			case QImage::Format_RGBA8888:
			case QImage::Format_RGBA8888_Premultiplied:
				return AV_PIX_FMT_RGBA;
			case QImage::Format_BGR30:
				return AV_PIX_FMT_X2BGR10;
			case QImage::Format_RGB30:
				return AV_PIX_FMT_X2RGB10;
			case QImage::Format_Alpha8:
			case QImage::Format_Grayscale8:
				return AV_PIX_FMT_GRAY8;
			case QImage::Format_RGBX64:
				return AV_PIX_FMT_RGB48;
			case QImage::Format_RGBA64:
			case QImage::Format_RGBA64_Premultiplied:
				return AV_PIX_FMT_RGBA64;
			case QImage::Format_Grayscale16:
				return AV_PIX_FMT_GRAY16;
			case QImage::Format_BGR888:
				return AV_PIX_FMT_BGR24;
			case QImage::Format_RGBX32FPx4:
				return AV_PIX_FMT_RGBF32;
			case QImage::Format_RGBA32FPx4:
			case QImage::Format_RGBA32FPx4_Premultiplied:
				return AV_PIX_FMT_RGBAF32;
			default:
				return AV_PIX_FMT_NONE;
			}
		}
		static av::PixelFormat getPixelFormat(const QImage& image) noexcept
		{
			return getPixelFormat(image.format());
		}

		// Find the best upconversion fit for the given QImage::Format that is compatible with AVPixelFormat.
		// As a reminder, FFmpeg doesn't care for premuiltiplied-or-not alpha
		static constexpr QImage::Format getBestFitForAVPixelFormatCompatible(const QImage::Format &format, bool optimizeForQPainter = false) noexcept
		{
			switch (format)
			{
				case QImage::Format_ARGB8565_Premultiplied:
				case QImage::Format_ARGB6666_Premultiplied:
				case QImage::Format_ARGB8555_Premultiplied:
				case QImage::Format_ARGB4444_Premultiplied:
					return QImage::Format_ARGB32_Premultiplied;
				case QImage::Format_RGB666:
				case QImage::Format_CMYK8888:
					return optimizeForQPainter ? QImage::Format_RGB32 : QImage::Format_RGB888;
				case QImage::Format_A2BGR30_Premultiplied:
				case QImage::Format_A2RGB30_Premultiplied:
					return QImage::Format_RGBA64_Premultiplied;
				case QImage::Format_RGBX16FPx4:
				case QImage::Format_RGBA16FPx4:
					return QImage::Format_RGBA32FPx4;
				case QImage::Format_RGBA16FPx4_Premultiplied:
					return QImage::Format_RGBA32FPx4_Premultiplied;
				default: // Format is already compatible with AVPixelFormat
					return format;
			}
		}

		static constexpr QImage::Format getQImageFormat(AVPixelFormat format) noexcept
		{
			switch (format) {
			case AV_PIX_FMT_MONOBLACK:
				return QImage::Format_Mono;
			case AV_PIX_FMT_MONOWHITE:
				return QImage::Format_MonoLSB;
			case AV_PIX_FMT_PAL8:
				return QImage::Format_Indexed8;
			case AV_PIX_FMT_RGB32:
				return QImage::Format_RGB32;
			case AV_PIX_FMT_ARGB:
				return QImage::Format_ARGB32;
			case AV_PIX_FMT_RGB565:
				return QImage::Format_RGB16;
			case AV_PIX_FMT_RGB555:
				return QImage::Format_RGB555;
			case AV_PIX_FMT_RGB24:
				return QImage::Format_RGB888;
			case AV_PIX_FMT_RGB444:
				return QImage::Format_RGB444;
			case AV_PIX_FMT_RGB0:
				return QImage::Format_RGBX8888;
			case AV_PIX_FMT_RGBA:
				return QImage::Format_RGBA8888;
			case AV_PIX_FMT_X2BGR10:
				return QImage::Format_BGR30;
			case AV_PIX_FMT_X2RGB10:
				return QImage::Format_RGB30;
			case AV_PIX_FMT_GRAY8:
				return QImage::Format_Grayscale8;
			case AV_PIX_FMT_RGB48:
				return QImage::Format_RGBX64;
			case AV_PIX_FMT_RGBA64:
				return QImage::Format_RGBA64;
			case AV_PIX_FMT_GRAY16:
				return QImage::Format_Grayscale16;
			case AV_PIX_FMT_BGR24:
				return QImage::Format_BGR888;
			case AV_PIX_FMT_RGBF32:
				return QImage::Format_RGBX32FPx4;
			case AV_PIX_FMT_RGBAF32:
				return QImage::Format_RGBA32FPx4;
			default:
				return QImage::Format_Invalid;
			}
		}

	// Slots for setting various properties of the video writer
	public slots:
		bool setFilename(const QDir& filename);
		bool setWidth(int width);
		bool setHeight(int height);
		bool setFPS(double fps);
		bool setPixelFormat(av::PixelFormat pixelFormat);
		bool setCodecID(AVCodecID codecID);

		// Throws: av::Exception
		void open();
		// Throws: av::Exception
		void writeFrame(QImage frame);
		// Closes the video writer and releases any resources.
		// Throws: av::Exception if there is an error during closing.
		void close();
	};
}