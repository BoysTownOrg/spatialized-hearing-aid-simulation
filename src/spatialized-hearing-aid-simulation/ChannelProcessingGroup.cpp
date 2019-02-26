#include "ChannelProcessingGroup.h"
#include <algorithm>

ChannelProcessingGroup::ChannelProcessingGroup(
	std::vector<channel_processing_type> processors
) noexcept :
	processors{ std::move(processors) } {}

void ChannelProcessingGroup::process(gsl::span<channel_type> audio) {
	auto channelsToProcess = std::min(
		gsl::narrow<processing_group_type::size_type>(audio.size()), 
		processors.size()
	);
	for (processing_group_type::size_type i{ 0 }; i < channelsToProcess; ++i)
		processors.at(i)->process(audio.at(i));
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
