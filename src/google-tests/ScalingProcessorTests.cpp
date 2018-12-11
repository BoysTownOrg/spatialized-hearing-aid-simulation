#include "assert-utility.h"
#include <signal-processing/ScalingProcessor.h>
#include <gtest/gtest.h>

class ScalingProcessorTestCase : public ::testing::TestCase {};

TEST(ScalingProcessorTestCase, processScalesAccordingToScalar) {
	ScalingProcessor processor{ 0.5 };
	std::vector<float> x{ 1, 2, 3 };
	processor.process({ x });
	assertEqual({ 1 * 0.5, 2 * 0.5, 3 * 0.5 }, x);
}