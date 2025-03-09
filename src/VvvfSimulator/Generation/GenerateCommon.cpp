#include "GenerateCommon.hpp"

namespace VvvfSimulator::Generation::GenerationCommon
{
	void GenerationVideoWriter::addEmptyFrames(size_t numFrames, bool darkMode)
	{
		QImage emptyImage(width(), height(), QImage::Format_ARGB32);
		emptyImage.fill(darkMode ? QColorConstants::Black : QColorConstants::White);
		for (auto i = numFrames; i-- > 0;) writeFrame(emptyImage);
	}
	
	void GenerationVideoWriter::addImageFrames(const QImage &image, size_t numFrames)
	{
		for (auto i = numFrames; i-- > 0;) writeFrame(image);
	}
}