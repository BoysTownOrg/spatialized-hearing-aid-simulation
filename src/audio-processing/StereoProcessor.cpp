#include "StereoProcessor.h"

StereoProcessor::StereoProcessor(
	std::shared_ptr<MonoProcessor> left, 
	std::shared_ptr<MonoProcessor> right
) :
	left{ std::move(left) },
	right{ std::move(right) } {}

void StereoProcessor::process(float * xLeft, float * xRight, int frameCount) {
	left->process(xLeft, frameCount);
	right->process(xRight, frameCount);
}
