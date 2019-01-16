#include "SignalProcessorStub.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <gtest/gtest.h>

class ChannelProcessingGroupTests : public ::testing::Test {
protected:
	std::vector<std::shared_ptr<SignalProcessorStub>> processors = makeStubs(3);
	ChannelProcessingGroup group{ { processors.begin(), processors.end() } };

private:
	static std::vector<std::shared_ptr<SignalProcessorStub>> makeStubs(int n) {
		std::vector<std::shared_ptr<SignalProcessorStub>> p;
		for (int i = 0; i < n; ++i)
			p.push_back(std::make_shared<SignalProcessorStub>());
		return p;
	}
};

TEST_F(ChannelProcessingGroupTests, processesChannelsInOrder) {
	const gsl::span<float> a{};
	const gsl::span<float> b{};
	const gsl::span<float> c{};
	std::vector<gsl::span<float>> channels{ a, b, c };
	group.process(channels);
	EXPECT_EQ(a, processors.at(0)->signal());
	EXPECT_EQ(b, processors.at(1)->signal());
	EXPECT_EQ(c, processors.at(2)->signal());
}

TEST_F(ChannelProcessingGroupTests, groupDelayReturnsMaxGroupDelay) {
	processors.at(0)->setGroupDelay(1);
	processors.at(1)->setGroupDelay(2);
	processors.at(2)->setGroupDelay(3);
	EXPECT_EQ(3, group.groupDelay());
}