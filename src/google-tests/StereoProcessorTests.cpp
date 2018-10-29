#include "MockSignalProcessor.h"
#include <signal-processing/DualSignalProcessor.h>
#include <gtest/gtest.h>

class StereoProcessorTestCase : public ::testing::TestCase {};

TEST(StereoProcessorTestCase, processProcessesLeftAndRight) {
	const auto left = std::make_shared<MockSignalProcessor>();
	const auto right = std::make_shared<MockSignalProcessor>();
	DualSignalProcessor processor{ left, right };
	float x{};
	float y{};
	processor.process(&x, &y, 1);
	EXPECT_EQ(&x, left->signal());
	EXPECT_EQ(&y, right->signal());
	EXPECT_EQ(1, left->frames());
	EXPECT_EQ(1, right->frames());
}
