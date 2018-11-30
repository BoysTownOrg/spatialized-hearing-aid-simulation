#pragma once

#include "audio-stream-processing-exports.h"
#include <audio-stream-processing/AudioFrameReader.h>

class ChannelCopier : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
public:
	AUDIO_STREAM_PROCESSING_API explicit ChannelCopier(
		std::shared_ptr<AudioFrameReader>
	);
	AUDIO_STREAM_PROCESSING_API void read(float ** channels, int frameCount) override;
};

