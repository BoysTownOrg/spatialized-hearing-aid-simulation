#include "DualSignalProcessor.h"

DualSignalProcessor::DualSignalProcessor(
	std::shared_ptr<SignalProcessor> left, 
	std::shared_ptr<SignalProcessor> right
) :
	left{ std::move(left) },
	right{ std::move(right) } {}

void DualSignalProcessor::process(float * xLeft, float * xRight, int frameCount) {
	left->process(xLeft, frameCount);
	right->process(xRight, frameCount);
}
