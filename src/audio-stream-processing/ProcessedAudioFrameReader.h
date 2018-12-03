#pragma once

#include "audio-stream-processing-exports.h"
#include "AudioFrameReader.h"
#include "AudioFrameProcessor.h"
#include <memory>

class ProcessedAudioFrameReader : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
	std::shared_ptr<AudioFrameProcessor> processor;
public:
	AUDIO_STREAM_PROCESSING_API ProcessedAudioFrameReader(
		std::shared_ptr<AudioFrameReader>,
		std::shared_ptr<AudioFrameProcessor>);
	AUDIO_STREAM_PROCESSING_API void read(float **, int) override;
	AUDIO_STREAM_PROCESSING_API bool complete() const override;
};

