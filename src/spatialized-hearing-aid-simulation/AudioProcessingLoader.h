#pragma once

#include "AudioLoader.h"

class AudioProcessingLoader : public AudioLoader {
public:
	virtual void reset() = 0;
};

class AudioProcessingLoaderFactory {
public:
	INTERFACE_OPERATIONS(AudioProcessingLoaderFactory);
	virtual std::shared_ptr<AudioProcessingLoader> make(
		std::shared_ptr<AudioFrameReader>,
		std::shared_ptr<AudioFrameProcessor>
	) = 0;
};