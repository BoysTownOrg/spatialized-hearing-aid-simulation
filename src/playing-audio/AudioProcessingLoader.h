#pragma once

#include "AudioFrameProcessor.h"
#include "AudioLoader.h"
#include "playing-audio-exports.h"
#include <audio-stream-processing/AudioFrameReader.h>

class AudioProcessingLoader : public AudioLoader {
	std::shared_ptr<AudioFrameProcessor> processor;
	std::shared_ptr<AudioFrameReader> reader;
	AudioFrameReaderFactory *readerFactory;
	AudioFrameProcessorFactory *processorFactory;
	long long paddedZeros{};
public:
	PLAYING_AUDIO_API AudioProcessingLoader(
		AudioFrameReaderFactory *,
		AudioFrameProcessorFactory *
	);
	PLAYING_AUDIO_API void prepare(Preparation) override;
	PLAYING_AUDIO_API void load(gsl::span<gsl::span<float>> audio) override;
	PLAYING_AUDIO_API bool complete() override;
	PLAYING_AUDIO_API int channels() override;
	PLAYING_AUDIO_API int sampleRate() override;

private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameProcessor> makeProcessor(
		AudioFrameProcessorFactory::Parameters
	);
};