#include <gtest/gtest.h>

class StereoProcessorAdapterTestCase : public ::testing::TestCase {};

TEST(StereoProcessorAdapterTestCase, tbd) {
	const auto processor = std::make_shared<MockStereoProcessor>();
	StereoProcessorAdapter adapter{ processor };
	float left{};
	float right{};
	float *channels[] = { &left, &right };
	adapter.process(channels, 1);
	EXPECT_EQ(&left, processor->left());
	EXPECT_EQ(&right, processor->right());
	EXPECT_EQ(1, processor->samples());
}