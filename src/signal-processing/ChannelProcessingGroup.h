#pragma once

#include "SignalProcessor.h"
#include "signal-processing-exports.h"
#include <playing-audio/RefactoredAudioFrameProcessor.h>
#include <vector>
#include <memory>

class ChannelProcessingGroup : public AudioFrameProcessor {
	std::vector<std::shared_ptr<SignalProcessor>> processors;
public:
	SIGNAL_PROCESSING_API explicit ChannelProcessingGroup(
		std::vector<std::shared_ptr<SignalProcessor>> processors
	);
	SIGNAL_PROCESSING_API void process(gsl::span<gsl::span<float>> audio) override;
	SIGNAL_PROCESSING_API int groupDelay() override;
	bool complete() override {
		return {};
	}
};

