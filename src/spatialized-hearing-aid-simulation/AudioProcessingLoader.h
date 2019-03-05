#pragma once

#include "AudioLoader.h"

class AudioProcessingLoaderFactory {
public:
	INTERFACE_OPERATIONS(AudioProcessingLoaderFactory);
	virtual std::shared_ptr<AudioLoader> make(
		std::shared_ptr<AudioFrameReader>,
		std::shared_ptr<AudioFrameProcessor>
	) = 0;
};