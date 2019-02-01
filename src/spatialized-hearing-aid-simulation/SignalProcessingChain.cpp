#include "SignalProcessingChain.h"
#include <algorithm>

void SignalProcessingChain::process(signal_type signal) {
	for (auto processor : processors)
		processor->process(signal);
}

void SignalProcessingChain::add(processing_element_type processor) {
	processors.push_back(std::move(processor));
}

auto SignalProcessingChain::groupDelay() -> index_type {
	return std::accumulate(
		processors.begin(),
		processors.end(),
		index_type{ 0 },
		[](index_type x, processing_element_type processor) { 
			return x + processor->groupDelay(); 
		}
	);
}