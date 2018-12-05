#include "SignalProcessingChain.h"

void SignalProcessingChain::process(float * x, int frames) {
	for (const auto &processor : processors)
		processor->process(x, frames);
}

void SignalProcessingChain::add(std::shared_ptr<SignalProcessor> processor) {
	processors.push_back(processor);
}
