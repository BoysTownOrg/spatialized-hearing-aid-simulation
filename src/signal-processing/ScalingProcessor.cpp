#include "ScalingProcessor.h"

ScalingProcessor::ScalingProcessor(float scale) : scale{ scale } {}

void ScalingProcessor::process(signal_type signal) {
	for (auto &x : signal)
		x *= scale;
}

auto ScalingProcessor::groupDelay() -> index_type {
	return 0;
}
