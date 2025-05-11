#include "QtVideoWriter.hpp"

// Copyright © 2025 VvvfGeeks, VVVF Systems
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later OR GPL-3.0-or-later

// QtVideoWriter.cpp
// Version 1.9.1.1

// Standard Library Includes
//#include <ctime>
// Package Includes
#include <avcpp/av.h> // Has already been included by the header above, but don't rely on indirect includes...
#include <avcpp/averror.h>
#include <QObject> // Has already been included by the declaration header, but don't rely on indirect includes...
#include <QtDebug>

namespace VvvfSimulator::Generation
{
	QtVideoWriter::QtVideoWriter(
		const std::filesystem::path& filename,
		const int width,
		const int height,
		const av::Rational &timeBase,
		const AVCodecID codecID,
		const av::PixelFormat pixelFormat,
		const av::Dictionary &options,
		bool openOnCreation,
		QObject* parent
	) :
		QObject(parent),
		m_filename(filename),
		m_options(options),
		m_width(width),
		m_height(height),
		m_timeBase(timeBase),
		m_formatContext(std::nullopt),
		m_codecContext(std::nullopt),
		m_stream(std::nullopt),
		m_pixelFormat(/*pixelFormat != AV_PIX_FMT_NONE && */getQImageFormat(pixelFormat.get()) == QImage::Format_Invalid ? av::PixelFormat(bestCompatiblePixelFormat) : pixelFormat),
		m_pts(0),
		m_codecID(codecID),
		m_isOpen(false)
	{
		if (openOnCreation)
			open();
	}

	QtVideoWriter::~QtVideoWriter()
	{
		close();
	}

	bool QtVideoWriter::setFilename(const std::filesystem::path &filename)
	{
		if (m_isOpen)
		{
			qWarning() << QObject::tr("Cannot change filename while the video writer is open");
			return false;
		}
		else
		{
			m_filename = filename;
			return true;
		}
	}

	bool QtVideoWriter::setWidth(int width)
	{
		if (m_isOpen)
		{
			qWarning() << QObject::tr("Cannot change width while the video writer is open");
			return false;
		}
		else
		{
			m_width = width;
			return true;
		}
	}

	bool QtVideoWriter::setHeight(int height)
	{
		if (m_isOpen)
		{
			qWarning() << QObject::tr("Cannot change height while the video writer is open");
			return false;
		}
		else
		{
			m_height = height;
			return true;
		}
	}

	bool QtVideoWriter::setTimeBase(const av::Rational &timeBase)
	{
		if (m_isOpen)
		{
			qWarning() << QObject::tr("Cannot change time base/FPS while the video writer is open");
			return false;
		}
		else
		{
			m_timeBase = timeBase;
			return true;
		}
	}

	bool QtVideoWriter::setPixelFormat(av::PixelFormat pixelFormat)
	{
		if (m_isOpen)
		{
			qWarning() << QObject::tr("Cannot change pixel format while the video writer is open");
			return false;
		}
		else
		{
			if (/*pixelFormat != AV_PIX_FMT_NONE && */getQImageFormat(pixelFormat.get()) == QImage::Format_Invalid) 
				m_pixelFormat = av::PixelFormat(bestCompatiblePixelFormat);
			else m_pixelFormat = pixelFormat;
			return true;
		}
	}

	bool QtVideoWriter::setCodecID(AVCodecID codecID)
	{
		if (m_isOpen)
		{
			qWarning() << QObject::tr("Cannot change codec ID while the video writer is open");
			return false;
		}
		else
		{
			m_codecID = codecID;
			return true;
		}
	}

	void QtVideoWriter::open()
	{
    if (m_isOpen) return;

    m_formatContext.emplace();
    m_formatContext->openOutput(m_filename.string(), m_options);
		/*
		if (ec)
		{
			qWarning() << QObject::tr("Failed to open video file: %1, error code: %2").arg(m_filename.path(), QString::number(((*ec).value())));
			m_isOpen = false;
			return false;
		}
			*/

		m_codecContext.emplace();
    m_codecContext->setWidth(m_width);
		m_codecContext->setHeight(m_height);
		m_codecContext->setPixelFormat(m_pixelFormat);
		m_codecContext->setTimeBase(m_timeBase);
    m_codecContext->open();

    m_stream.emplace(std::move(m_formatContext->addStream(reinterpret_cast<const av::VideoEncoderContext &>(*m_codecContext))));
    m_formatContext->writeHeader();

    //m_frame = av::VideoFrame(m_codecContext->pixelFormat(), m_codecContext->width(), m_codecContext->height());
    //m_packet.emplace();

    m_pts = 0;
		m_isOpen = true;
		//return true;
	}

	void QtVideoWriter::writeFrame(QImage image)
	{
    if (!m_isOpen) return;
		const auto conditionalImageScale = [&]()
		{
			// Resize the QImage to match the codec context dimensions
			if (image.width() != m_codecContext->width() || image.height() != m_codecContext->height())
			{
				image = std::move(image.scaled(m_codecContext->width(), m_codecContext->height()));
			}
		};

		const QImage::Format contextImageFormat = getQImageFormat(m_codecContext->pixelFormat().get());
		const auto conditionalImageConvert = [&]()
		{
			if (image.format() != contextImageFormat)
			{
				image.convertTo(contextImageFormat);
			}
		};

		// Convert the image format and scale it if necessary
		// The order of operations is important to avoid unnecessary conversions
		if (image.format() < contextImageFormat)
		{
			conditionalImageConvert();
			conditionalImageScale();
		}
		else
		{
			conditionalImageScale();
			conditionalImageConvert();
		}

    Q_ASSERT(image.width() == m_codecContext->width());
		Q_ASSERT(image.height() == m_codecContext->height());
		av::VideoFrame frame(image.bits(), image.sizeInBytes(), m_codecContext->pixelFormat().get(), m_codecContext->width(), m_codecContext->height());
    frame.setPts(m_pts++, m_codecContext->timeBase());

    m_packet.emplace(std::move(static_cast<av::VideoEncoderContext*>(&(m_codecContext.value()))->encode(frame)));
    m_formatContext->writePacket(*m_packet);
	}

	void QtVideoWriter::close()
	{
    if (!m_isOpen) return; // Check if the video writer is already closed

		if (m_formatContext)
		{
			//static_cast<av::VideoEncoderContext*>(m_codecContext.get())->flush(m_packet);
			m_formatContext->flush();
			m_formatContext->writePacket(*m_packet);
			m_formatContext->writeTrailer();
    }
		m_isOpen = false;
		m_packet.reset();
		m_codecContext.reset();
		m_formatContext.reset();
		m_stream.reset();
	}

	void QtVideoWriter::addEmptyFrames(size_t numFrames, bool darkMode)
	{
		QImage emptyImage(width(), height(), QImage::Format_ARGB32);
		emptyImage.fill(darkMode ? QColorConstants::Black : QColorConstants::White);
		for (auto i = numFrames; i-- > 0;) writeFrame(emptyImage);
	}
	
	void QtVideoWriter::addImageFrames(const QImage &image, size_t numFrames)
	{
		for (auto i = numFrames; i-- > 0;) writeFrame(image);
	}
}