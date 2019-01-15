#pragma once

#include "signal-processing-exports.h"
#include <signal-processing/SignalProcessor.h>

class ScalingProcessor : public SignalProcessor {
	float scale;
public:
	SIGNAL_PROCESSING_API explicit ScalingProcessor(float scale);
	SIGNAL_PROCESSING_API void process(gsl::span<float> signal) override;
	int groupDelay() override;
};

