#pragma once

#include "signal-processing-exports.h"
#include "SignalProcessor.h"
#include "StereoProcessor.h"
#include <memory>

class DualSignalProcessor : public StereoProcessor {
	std::shared_ptr<SignalProcessor> left;
	std::shared_ptr<SignalProcessor> right;
public:
	SIGNAL_PROCESSING_API DualSignalProcessor(
		std::shared_ptr<SignalProcessor> left,
		std::shared_ptr<SignalProcessor> right);
	SIGNAL_PROCESSING_API void process(float *, float *, int) override;
};

