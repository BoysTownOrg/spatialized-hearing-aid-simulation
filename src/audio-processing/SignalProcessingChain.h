#pragma once

#include "audio-processing-exports.h"
#include "SignalProcessor.h"
#include <memory>
#include <vector>

class SignalProcessingChain : public SignalProcessor {
	std::vector<std::shared_ptr<SignalProcessor>> processors{};
public:
	AUDIO_PROCESSING_API void process(float *x, int) override;
	AUDIO_PROCESSING_API void add(std::shared_ptr<SignalProcessor> processor);
};
