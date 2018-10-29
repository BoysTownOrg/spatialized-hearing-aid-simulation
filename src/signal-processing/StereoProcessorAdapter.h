#pragma once

#include "signal-processing-exports.h"
#include "StereoProcessor.h"
#include <audio-stream-processing/AudioProcessor.h>
#include <memory>

class StereoProcessorAdapter : public AudioProcessor {
	std::shared_ptr<StereoProcessor> processor;
public:
	SIGNAL_PROCESSING_API explicit StereoProcessorAdapter(
		std::shared_ptr<StereoProcessor> processor);
	SIGNAL_PROCESSING_API void process(float **channels, int frameCount) override;
};

