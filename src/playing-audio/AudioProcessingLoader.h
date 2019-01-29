#pragma once

#include "AudioFrameProcessor.h"
#include "AudioLoader.h"
#include "playing-audio-exports.h"
#include <audio-stream-processing/AudioFrameReader.h>

class AudioProcessingLoader : public AudioLoader {
	std::shared_ptr<AudioFrameProcessor> processor;
	std::shared_ptr<AudioFrameReader> reader;
	AudioFrameReaderFactory *readerFactory;
	long long paddedZeros{};
public:
	explicit PLAYING_AUDIO_API AudioProcessingLoader(
		AudioFrameReaderFactory *
	);
	PLAYING_AUDIO_API void prepare(Preparation) override;
	PLAYING_AUDIO_API void load(gsl::span<gsl::span<float>> audio) override;
	PLAYING_AUDIO_API bool complete() override;
	PLAYING_AUDIO_API int channels() override;
	PLAYING_AUDIO_API int sampleRate() override;
	PLAYING_AUDIO_API void updateProcessor(std::shared_ptr<AudioFrameProcessor>);

private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
};