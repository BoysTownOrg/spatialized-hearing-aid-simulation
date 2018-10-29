#pragma once

#include "audio-processing-exports.h"
#include "MonoProcessor.h"
#include <memory>
#include <vector>

class MonoProcessingChain : public MonoProcessor {
	std::vector<std::shared_ptr<MonoProcessor>> processors{};
public:
	AUDIO_PROCESSING_API void process(float *x, int) override;
	AUDIO_PROCESSING_API void add(std::shared_ptr<MonoProcessor> processor);
};
