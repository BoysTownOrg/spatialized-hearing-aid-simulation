#pragma once

#include "signal-processing-exports.h"
#include <spatialized-hearing-aid-simulation/SignalProcessor.h>

class ScalingProcessor : public SignalProcessor {
	float scale;
public:
	SIGNAL_PROCESSING_API explicit ScalingProcessor(float scale);
	SIGNAL_PROCESSING_API void process(signal_type signal) override;
	SIGNAL_PROCESSING_API index_type groupDelay() override;
};

