#pragma once

#include "SignalProcessor.h"
#include "signal-processing-exports.h"

class ScalingProcessor : public SignalProcessor {
	float scale;
public:
	SIGNAL_PROCESSING_API explicit ScalingProcessor(float scale);
	SIGNAL_PROCESSING_API void process(gsl::span<float> signal) override;
	SIGNAL_PROCESSING_API index_type groupDelay() override;
};

