#pragma once
#include <common-includes/Interface.h>

class MonoProcessor {
public:
	INTERFACE_OPERATIONS(MonoProcessor);
	virtual void process(float *, int) = 0;
};

#ifdef AUDIO_PROCESSING_EXPORTS
	#define AUDIO_PROCESSING_API __declspec(dllexport)
#else
	#define AUDIO_PROCESSING_API __declspec(dllimport)
#endif

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

