#pragma once

#include "signal-processing-exports.h"
#include "StereoProcessor.h"
#include <audio-stream-processing/AudioFrameProcessor.h>
#include <memory>

class StereoProcessorAdapter : public AudioFrameProcessor {
	std::shared_ptr<StereoProcessor> processor;
public:
	SIGNAL_PROCESSING_API explicit StereoProcessorAdapter(
		std::shared_ptr<StereoProcessor>);
	SIGNAL_PROCESSING_API void process(float **, int) override;
};
