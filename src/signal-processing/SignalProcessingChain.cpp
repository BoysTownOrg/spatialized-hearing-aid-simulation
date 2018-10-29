#include "SignalProcessingChain.h"

void SignalProcessingChain::process(float * x, int frameCount) {
	for (const auto &processor : processors)
		processor->process(x, frameCount);
}

void SignalProcessingChain::add(std::shared_ptr<SignalProcessor> processor) {
	processors.push_back(processor);
}
