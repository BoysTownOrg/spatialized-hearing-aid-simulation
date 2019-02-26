#include "ChannelProcessingGroup.h"
#include <algorithm>

ChannelProcessingGroup::ChannelProcessingGroup(
	std::vector<channel_processing_type> processors
) :
	processors{ std::move(processors) } {}

void ChannelProcessingGroup::process(gsl::span<channel_type> audio) {
	auto it = processors.begin();
	for (auto channel : audio)
		(*it++)->process(channel);
}

auto ChannelProcessingGroup::groupDelay() -> channel_type::index_type {
	if (processors.size() == 0)
		return 0;

	auto maximumDelayedProcessor = std::max_element(
		processors.begin(),
		processors.end(),
		[](channel_processing_type a, channel_processing_type b) { 
			return a->groupDelay() < b->groupDelay(); 
		}
	);
	return (*maximumDelayedProcessor)->groupDelay();
}
