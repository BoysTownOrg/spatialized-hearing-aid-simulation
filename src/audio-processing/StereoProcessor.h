#pragma once

#include "audio-processing-exports.h"
#include "MonoProcessor.h"
#include <memory>

class StereoProcessor {
	std::shared_ptr<MonoProcessor> left;
	std::shared_ptr<MonoProcessor> right;
public:
	AUDIO_PROCESSING_API StereoProcessor(
		std::shared_ptr<MonoProcessor> left,
		std::shared_ptr<MonoProcessor> right);
	AUDIO_PROCESSING_API void process(float *xLeft, float *xRight, int frameCount);
};

