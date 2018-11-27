#include <gtest/gtest.h>

class ChannelProcessorTestCase : public ::testing::TestCase {};

TEST(ChannelProcessorTestCase, tbd) {
	std::vector<MockSignalProcessor> processors(3);
	ChannelProcessingGroup group{ processors };
	float a{};
	float b{};
	float c{};
	float *channels[] = { &a, &b, &c };
	group->process(channels, 0);
	EXPECT_EQ(&a, processors[0]->x());
	EXPECT_EQ(&b, processors[1]->x());
	EXPECT_EQ(&c, processors[2]->x());
}