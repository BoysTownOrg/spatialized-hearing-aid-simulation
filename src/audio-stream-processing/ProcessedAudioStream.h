#pragma once

#ifdef AUDIO_STREAM_PROCESSING_EXPORTS
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllexport)
#else
	#define AUDIO_STREAM_PROCESSING_API __declspec(dllimport)
#endif

#include "AudioReader.h"
#include "AudioProcessor.h"
#include <audio-stream-processing/AudioStream.h>
#include <memory>

class ProcessedAudioStream : public AudioStream {
	std::shared_ptr<AudioReader> reader;
	std::shared_ptr<AudioProcessor> processor;
public:
	AUDIO_STREAM_PROCESSING_API ProcessedAudioStream(
		std::shared_ptr<AudioReader> reader,
		std::shared_ptr<AudioProcessor> processor);
	AUDIO_STREAM_PROCESSING_API void fillBuffer(float **channels, int) override;
};

