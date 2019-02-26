#pragma once

#include "SignalProcessor.h"
#include "AudioFrameProcessor.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <vector>
#include <memory>

class ChannelProcessingGroup : public AudioFrameProcessor {
public:
	using channel_processing_type = std::shared_ptr<SignalProcessor>;
	using processing_group_type = std::vector<channel_processing_type>;

	SPATIALIZED_HA_SIMULATION_API explicit ChannelProcessingGroup(
		processing_group_type processors
	) noexcept;
	SPATIALIZED_HA_SIMULATION_API void process(gsl::span<channel_type> audio) override;
	SPATIALIZED_HA_SIMULATION_API channel_type::index_type groupDelay() override;
private:
	processing_group_type processors;
};

