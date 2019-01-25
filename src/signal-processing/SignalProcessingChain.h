#pragma once

#include "SignalProcessor.h"
#include "signal-processing-exports.h"
#include <memory>
#include <vector>

class SignalProcessingChain : public SignalProcessor {
public:
	using processing_element_type = std::shared_ptr<SignalProcessor>;
	SIGNAL_PROCESSING_API void process(signal_type signal) override;
	SIGNAL_PROCESSING_API void add(processing_element_type);
	SIGNAL_PROCESSING_API index_type groupDelay() override;
private:
	std::vector<processing_element_type> processors{};
};
