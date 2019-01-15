#include "SignalProcessingChain.h"
#include <algorithm>

void SignalProcessingChain::process(gsl::span<float> signal) {
	for (const auto &processor : processors)
		processor->process(signal);
}

void SignalProcessingChain::add(std::shared_ptr<SignalProcessor> processor) {
	processors.push_back(processor);
}

int SignalProcessingChain::groupDelay() {
	return std::accumulate(
		processors.begin(),
		processors.end(),
		0,
		[](int x, std::shared_ptr<SignalProcessor> processor) { 
			return x + processor->groupDelay(); 
		}
	);
}