#pragma once

#include "SignalProcessor.h"
#include "AudioFrameProcessor.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <vector>
#include <memory>

class ChannelProcessingGroup : public AudioFrameProcessor {
public:
	using channel_processing_type = std::shared_ptr<SignalProcessor>;
	SPATIALIZED_HA_SIMULATION_API explicit ChannelProcessingGroup(
		std::vector<channel_processing_type> processors
	);
	SPATIALIZED_HA_SIMULATION_API void process(gsl::span<channel_type> audio) override;
	SPATIALIZED_HA_SIMULATION_API channel_type::index_type groupDelay() override;
private:
	std::vector<channel_processing_type> processors;
};

