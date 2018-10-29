#include "MonoProcessingChain.h"

void MonoProcessingChain::process(float * x, int frameCount) {
	for (const auto &processor : processors)
		processor->process(x, frameCount);
}

void MonoProcessingChain::add(std::shared_ptr<SignalProcessor> processor) {
	processors.push_back(processor);
}
