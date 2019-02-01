#pragma once

#include "SignalProcessor.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <memory>
#include <vector>

class SignalProcessingChain : public SignalProcessor {
public:
	using processing_element_type = std::shared_ptr<SignalProcessor>;
	SPATIALIZED_HA_SIMULATION_API void process(signal_type signal) override;
	SPATIALIZED_HA_SIMULATION_API void add(processing_element_type);
	SPATIALIZED_HA_SIMULATION_API index_type groupDelay() override;
private:
	std::vector<processing_element_type> processors{};
};
