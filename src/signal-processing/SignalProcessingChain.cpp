#include "SignalProcessingChain.h"

void SignalProcessingChain::process(gsl::span<float> signal) {
	for (const auto &processor : processors)
		processor->process(signal);
}

void SignalProcessingChain::add(std::shared_ptr<SignalProcessor> processor) {
	processors.push_back(processor);
}
