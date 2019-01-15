#include "SignalProcessorStub.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <gtest/gtest.h>

class ChannelProcessingGroupTests : public ::testing::Test {
protected:
	std::vector<std::shared_ptr<SignalProcessorStub>> processors{
		std::make_shared<SignalProcessorStub>(),
		std::make_shared<SignalProcessorStub>(),
		std::make_shared<SignalProcessorStub>()
	};
	ChannelProcessingGroup group{ { processors.begin(), processors.end() } };
};

TEST_F(ChannelProcessingGroupTests, processesChannelsInOrder) {
	gsl::span<float> a{};
	gsl::span<float> b{};
	gsl::span<float> c{};
	std::vector<gsl::span<float>> channels{ a, b, c };
	group.process(channels);
	EXPECT_EQ(a, processors[0]->signal());
	EXPECT_EQ(b, processors[1]->signal());
	EXPECT_EQ(c, processors[2]->signal());
}

TEST_F(ChannelProcessingGroupTests, groupDelayReturnsMaxGroupDelay) {
	processors[0]->setGroupDelay(1);
	processors[1]->setGroupDelay(2);
	processors[2]->setGroupDelay(3);
	EXPECT_EQ(3, group.groupDelay());
}