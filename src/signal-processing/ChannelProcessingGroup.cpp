#include "ChannelProcessingGroup.h"
#include <algorithm>

ChannelProcessingGroup::ChannelProcessingGroup(
	std::vector<std::shared_ptr<SignalProcessor>> processors
) :
	processors{ std::move(processors) } {}

void ChannelProcessingGroup::process(gsl::span<channel_type> audio) {
	using size_type = decltype(processors)::size_type;
	for (size_type i = 0; i < processors.size(); ++i)
		processors[i]->process(audio[i]);
}

auto ChannelProcessingGroup::groupDelay() -> channel_type::index_type {
	return (*std::max_element(
		processors.begin(),
		processors.end(),
		[](std::shared_ptr<SignalProcessor> a, std::shared_ptr<SignalProcessor> b) { 
			return a->groupDelay() < b->groupDelay(); 
		}
	))->groupDelay();
}
