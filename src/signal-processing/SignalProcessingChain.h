#pragma once

#include "SignalProcessor.h"
#include "signal-processing-exports.h"
#include <memory>
#include <vector>

class SignalProcessingChain : public SignalProcessor {
	std::vector<std::shared_ptr<SignalProcessor>> processors{};
public:
	SIGNAL_PROCESSING_API void process(signal_type signal) override;
	SIGNAL_PROCESSING_API void add(std::shared_ptr<SignalProcessor>);
	SIGNAL_PROCESSING_API index_type groupDelay() override;
};
