#pragma once

#include "signal-processing-exports.h"
#include <playing-audio/AudioFrameProcessor.h>
#include <signal-processing/SignalProcessor.h>
#include <vector>
#include <memory>

class ChannelProcessingGroup : public AudioFrameProcessor {
	std::vector<std::shared_ptr<SignalProcessor>> processors;
public:
	SIGNAL_PROCESSING_API explicit ChannelProcessingGroup(
		std::vector<std::shared_ptr<SignalProcessor>> processors
	);
	SIGNAL_PROCESSING_API void process(float **channels, int) override;
};

