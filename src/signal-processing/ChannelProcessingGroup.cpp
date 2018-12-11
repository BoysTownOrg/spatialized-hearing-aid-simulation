#include "ChannelProcessingGroup.h"

ChannelProcessingGroup::ChannelProcessingGroup(
	std::vector<std::shared_ptr<SignalProcessor>> processors
) :
	processors{ std::move(processors) } {}

void ChannelProcessingGroup::process(gsl::span<float *> channels, int frames) {
	using size_type = std::vector<std::shared_ptr<SignalProcessor>>::size_type;
	for (size_type i = 0; i < processors.size(); ++i)
		processors[i]->process({ channels[i], frames });
}
