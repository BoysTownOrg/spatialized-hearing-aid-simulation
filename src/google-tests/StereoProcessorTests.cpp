#include <gtest/gtest.h>

class StereoProcessorTestCase : public ::testing::TestCase {};

TEST(StereoProcessorTestCase, tbd) {
	const auto left = std::make_shared<MockMonoProcessor>();
	const auto right = std::make_shared<MockMonoProcessor>();
	StereoProcessor stereo{ left, right };
	float x{};
	float y{};
	stereo.process(&x, &y, 1);
	EXPECT_EQ(&x, left->signal());
	EXPECT_EQ(&y, right->signal());
	EXPECT_EQ(1, left->frames());
	EXPECT_EQ(1, right->frames());
}