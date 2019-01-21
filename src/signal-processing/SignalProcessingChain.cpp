#include "SignalProcessingChain.h"
#include <algorithm>

void SignalProcessingChain::process(gsl::span<float> signal) {
	for (auto processor : processors)
		processor->process(signal);
}

void SignalProcessingChain::add(std::shared_ptr<SignalProcessor> processor) {
	processors.push_back(std::move(processor));
}

auto SignalProcessingChain::groupDelay() -> index_type {
	return std::accumulate(
		processors.begin(),
		processors.end(),
		index_type{ 0 },
		[](index_type x, std::shared_ptr<SignalProcessor> processor) { 
			return x + processor->groupDelay(); 
		}
	);
}