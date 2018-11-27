#include "ScalingProcessor.h"

ScalingProcessor::ScalingProcessor(float scale) : scale{ scale } {}

void ScalingProcessor::process(float * signal, int samples) {
	for (int i = 0; i < samples; ++i)
		signal[i] *= scale;
}
