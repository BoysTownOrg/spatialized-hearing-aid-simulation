#pragma once

#ifdef AUDIO_STREAM_PROCESSING_EXPORTS
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllexport)
#else
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllimport)
#endif

#include "AudioFrameReader.h"
#include "AudioProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <memory>

class ProcessedAudioStream : public AudioFrameReader {
	std::shared_ptr<AudioFrameReader> reader;
	std::shared_ptr<AudioProcessor> processor;
public:
	AUDIO_STREAM_PROCESSING_API ProcessedAudioStream(
		std::shared_ptr<AudioFrameReader> reader,
		std::shared_ptr<AudioProcessor> processor);
	AUDIO_STREAM_PROCESSING_API void read(float **channels, int) override;
};

