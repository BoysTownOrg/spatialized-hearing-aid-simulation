#pragma once

#include "signal-processing-exports.h"
#include "SignalProcessor.h"
#include <memory>

class StereoProcessor {
	std::shared_ptr<SignalProcessor> left;
	std::shared_ptr<SignalProcessor> right;
public:
	AUDIO_PROCESSING_API StereoProcessor(
		std::shared_ptr<SignalProcessor> left,
		std::shared_ptr<SignalProcessor> right);
	AUDIO_PROCESSING_API void process(float *xLeft, float *xRight, int frameCount);
};

