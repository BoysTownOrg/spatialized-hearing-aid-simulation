#pragma once

#include "SignalProcessor.h"
#include "signal-processing-exports.h"
#include <spatialized-hearing-aid-simulation/AudioFrameProcessor.h>
#include <vector>
#include <memory>

class ChannelProcessingGroup : public AudioFrameProcessor {
public:
	using channel_processing_type = std::shared_ptr<SignalProcessor>;
	SIGNAL_PROCESSING_API explicit ChannelProcessingGroup(
		std::vector<channel_processing_type> processors
	);
	SIGNAL_PROCESSING_API void process(gsl::span<channel_type> audio) override;
	SIGNAL_PROCESSING_API channel_type::index_type groupDelay() override;
private:
	std::vector<channel_processing_type> processors;
};

