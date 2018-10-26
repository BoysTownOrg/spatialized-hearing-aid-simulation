#pragma once

#ifdef AUDIO_PROCESSING_EXPORTS
	#define AUDIO_PROCESSING_API __declspec(dllexport)
#else
	#define AUDIO_PROCESSING_API __declspec(dllimport)
#endif

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

