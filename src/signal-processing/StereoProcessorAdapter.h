#pragma once

#include <common-includes/Interface.h>

class StereoProcessor {
public:
	INTERFACE_OPERATIONS(StereoProcessor);
	virtual void process(float *xLeft, float *xRight, int samples) = 0;
};

#include <audio-stream-processing/AudioProcessor.h>
#include <memory>

class StereoProcessorAdapter : public AudioProcessor {
	std::shared_ptr<StereoProcessor> processor;
public:
	explicit StereoProcessorAdapter(
		std::shared_ptr<StereoProcessor> processor
	) :
		processor{ std::move(processor) } {}

	void process(float **channels, int frameCount) override {
		processor->process(channels[0], channels[1], frameCount);
	}
};

