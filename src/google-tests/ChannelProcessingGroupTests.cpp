#include "SignalProcessorStub.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <gtest/gtest.h>

class ChannelProcessingGroupTestCase : public ::testing::TestCase {};

TEST(ChannelProcessingGroupTestCase, processesChannelsInOrder) {
	std::vector<std::shared_ptr<SignalProcessorStub>> processors{
		std::make_shared<SignalProcessorStub>(),
		std::make_shared<SignalProcessorStub>(),
		std::make_shared<SignalProcessorStub>()
	};
	ChannelProcessingGroup group{ { processors[0], processors[1], processors[2] } };
	gsl::span<float> a{};
	gsl::span<float> b{};
	gsl::span<float> c{};
	std::vector<gsl::span<float>> channels{ a, b, c };
	group.process(channels);
	EXPECT_EQ(a, processors[0]->signal());
	EXPECT_EQ(b, processors[1]->signal());
	EXPECT_EQ(c, processors[2]->signal());
}

TEST(ChannelProcessingGroupTestCase, groupDelayReturnsMaxGroupDelay) {
	auto processor1 = std::make_shared<SignalProcessorStub>();
	processor1->setGroupDelay(1);
	auto processor2 = std::make_shared<SignalProcessorStub>();
	processor2->setGroupDelay(2);
	auto processor3 = std::make_shared<SignalProcessorStub>();
	processor3->setGroupDelay(3);
	std::vector<std::shared_ptr<SignalProcessorStub>> processors{
		processor1,
		processor2,
		processor3
	};
	ChannelProcessingGroup group{ { processors[0], processors[1], processors[2] } };
	EXPECT_EQ(3, group.groupDelay());
}