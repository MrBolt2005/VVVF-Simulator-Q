#include "GenerateCommon.hpp"

namespace VvvfSimulator::Generation::GenerationCommon
{
	LibAVCodecText::LibAVCodecText(const av::Codec &codec)
		: id(codec.id())
		, type(codec.type())
		, name(codec.name())
		, longName(codec.longName())
		, isDecoder(codec.isDecoder())
	{
		if (const auto desc = avcodec_descriptor_get(id); desc)
		{
			props = desc->props;
			if (const auto mimes = desc->mime_types; mimes)
			{
				qsizetype i;
				for (i = 0; mimes[i] != NULL; i++);
				mimeTypes.reserve(i);
				for (i = 0; mimes[i] != NULL; i++) mimeTypes.emplace_back(mimes[i]);
			}
		}

		const auto framerates = codec.supportedFramerates();
		if (const auto x = framerates.size(); x != 0) supportedFramerates.reserve(x);
		for (const auto &rate : codec.supportedFramerates())
			supportedFramerates.emplace_back(1.0 / rate.getDouble());

		const auto pixFmts = codec.supportedPixelFormats();
		if (const auto x = pixFmts.size(); x != 0) supportedPixelFormats.reserve(x);
		for (const auto &fmt : pixFmts)
			supportedPixelFormats.push_back({fmt.get(), fmt.name()});
			
		const auto samplerates = codec.supportedSamplerates();
		supportedSamplerates.assign(samplerates.cbegin(), samplerates.cend());

		const auto sampleFmts = codec.supportedSampleFormats();
		if (const auto x = sampleFmts.size(); x != 0) supportedSampleFormats.reserve(x);
		for (const auto &fmt : sampleFmts)
			supportedSampleFormats.push_back({fmt.get(), fmt.name()});
	}
}