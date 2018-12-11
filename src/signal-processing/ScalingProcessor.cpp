#include "ScalingProcessor.h"

ScalingProcessor::ScalingProcessor(float scale) : scale{ scale } {}

void ScalingProcessor::process(gsl::span<float> signal) {
	for (auto &x : signal)
		x *= scale;
}
