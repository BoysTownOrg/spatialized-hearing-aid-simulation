#pragma once

#include <audio-stream-processing/AudioFrameReader.h>

class ChannelCopier : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
public:
	explicit ChannelCopier(
		std::shared_ptr<AudioFrameReader> reader
	) :
		reader{ std::move(reader) } {}

	void read(float ** channels, int frameCount) override
	{
		reader->read(channels, frameCount);
		for (int i = 0; i < frameCount; ++i)
			channels[1][i] = channels[0][i];
	}
};

