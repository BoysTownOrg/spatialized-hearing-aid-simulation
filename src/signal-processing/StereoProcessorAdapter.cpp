#include "StereoProcessorAdapter.h"

StereoProcessorAdapter::StereoProcessorAdapter(
	std::shared_ptr<StereoProcessor> processor
) :
	processor{ std::move(processor) } {}

void StereoProcessorAdapter::process(float ** channels, int frameCount) {
	processor->process(channels[0], channels[1], frameCount);
}
