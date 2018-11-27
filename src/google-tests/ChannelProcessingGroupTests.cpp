#include "MockSignalProcessor.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <gtest/gtest.h>

class ChannelProcessingGroupTestCase : public ::testing::TestCase {};

TEST(ChannelProcessingGroupTestCase, tbd) {
	std::vector<std::shared_ptr<MockSignalProcessor>> processors;
	processors.push_back(std::make_shared<MockSignalProcessor>());
	processors.push_back(std::make_shared<MockSignalProcessor>());
	processors.push_back(std::make_shared<MockSignalProcessor>());
	ChannelProcessingGroup group{ { processors[0], processors[1], processors[2] } };
	float a{};
	float b{};
	float c{};
	float *channels[] = { &a, &b, &c };
	group.process(channels, 0);
	EXPECT_EQ(&a, processors[0]->signal());
	EXPECT_EQ(&b, processors[1]->signal());
	EXPECT_EQ(&c, processors[2]->signal());
}