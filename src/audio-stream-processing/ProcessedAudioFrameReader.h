#pragma once

#ifdef AUDIO_STREAM_PROCESSING_EXPORTS
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllexport)
#else
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllimport)
#endif

#include "AudioFrameReader.h"
#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <memory>

class ProcessedAudioFrameReader : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
	std::shared_ptr<AudioFrameProcessor> processor;
public:
	AUDIO_STREAM_PROCESSING_API ProcessedAudioFrameReader(
		std::shared_ptr<AudioFrameReader>,
		std::shared_ptr<AudioFrameProcessor>);
	AUDIO_STREAM_PROCESSING_API void read(float **, int) override;
};

