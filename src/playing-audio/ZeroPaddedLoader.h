#pragma once

#include "playing-audio-exports.h"
#include <spatialized-hearing-aid-simulation/AudioProcessingLoader.h>

class ZeroPaddedLoader : public AudioProcessingLoader {
	std::shared_ptr<AudioFrameProcessor> processor;
	std::shared_ptr<AudioFrameReader> reader;
	long long paddedZeros{};
public:
	PLAYING_AUDIO_API ZeroPaddedLoader(
		std::shared_ptr<AudioFrameReader> reader,
		std::shared_ptr<AudioFrameProcessor> processor
	) noexcept;
	PLAYING_AUDIO_API void reset() override;
	PLAYING_AUDIO_API void load(gsl::span<channel_type> audio) override;
	PLAYING_AUDIO_API bool complete() override;
	PLAYING_AUDIO_API void setReader(std::shared_ptr<AudioFrameReader>) override;
	PLAYING_AUDIO_API void setProcessor(std::shared_ptr<AudioFrameProcessor>) override;
private:
	void padZeros(gsl::span<channel_type> audio, long long zerosToPad);
};

class ZeroPaddedLoaderFactory : public AudioProcessingLoaderFactory {
	std::shared_ptr<AudioProcessingLoader> make(
		std::shared_ptr<AudioFrameReader>, 
		std::shared_ptr<AudioFrameProcessor>
	) override;
};