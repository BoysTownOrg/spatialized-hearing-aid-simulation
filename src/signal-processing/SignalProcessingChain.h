#pragma once

#include "signal-processing-exports.h"
#include "SignalProcessor.h"
#include <memory>
#include <vector>

class SignalProcessingChain : public SignalProcessor {
	std::vector<std::shared_ptr<SignalProcessor>> processors{};
public:
	SIGNAL_PROCESSING_API void process(float *x, int) override;
	SIGNAL_PROCESSING_API void add(std::shared_ptr<SignalProcessor> processor);
};
