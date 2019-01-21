#include "assert-utility.h"
#include <signal-processing/ScalingProcessor.h>
#include <gtest/gtest.h>

TEST(ScalingProcessorTests, processScalesAccordingToScalar) {
	ScalingProcessor processor{ 0.5 };
	std::vector<float> x{ 1, 2, 3 };
	processor.process(x);
	assertEqual({ 1 * 0.5, 2 * 0.5, 3 * 0.5 }, x);
}

TEST(ScalingProcessorTests, groupDelayReturnsZero) {
	ScalingProcessor processor{ {} };
	EXPECT_EQ(0, processor.groupDelay());
};
