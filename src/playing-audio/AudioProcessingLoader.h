#pragma once

#include "AudioLoader.h"
#include "playing-audio-exports.h"

class AudioProcessingLoader : public AudioLoader {
	std::shared_ptr<AudioFrameProcessor> processor;
	std::shared_ptr<AudioFrameReader> reader;
	long long paddedZeros{};
public:
	PLAYING_AUDIO_API AudioProcessingLoader();
	PLAYING_AUDIO_API void reset() override;
	PLAYING_AUDIO_API void load(gsl::span<gsl::span<float>> audio) override;
	PLAYING_AUDIO_API bool complete() override;
	PLAYING_AUDIO_API void setReader(std::shared_ptr<AudioFrameReader>) override;
	PLAYING_AUDIO_API void setProcessor(std::shared_ptr<AudioFrameProcessor>) override;
};