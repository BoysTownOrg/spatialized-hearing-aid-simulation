#include "SignalProcessorStub.h"
#include "assert-utility.h"
#include <spatialized-hearing-aid-simulation/ChannelProcessingGroup.h>
#include <gtest/gtest.h>

namespace {
	class ChannelProcessingGroupTests : public ::testing::Test {
	protected:
		using channel_type = ChannelProcessingGroup::channel_type;
		using buffer_type = std::vector<channel_type::element_type>;
		std::vector<std::shared_ptr<SignalProcessorStub>> processors{};
		std::vector<channel_type> channels{};

		ChannelProcessingGroup construct() {
			return ChannelProcessingGroup{ { processors.begin(), processors.end() } };
		}

		void assignStubs(int n) {
			processors = makeStubs(n);
		}

	private:
		static std::vector<std::shared_ptr<SignalProcessorStub>> makeStubs(int n) {
			std::vector<std::shared_ptr<SignalProcessorStub>> p;
			for (int i = 0; i < n; ++i)
				p.push_back(std::make_shared<SignalProcessorStub>());
			return p;
		}
	};

	TEST_F(ChannelProcessingGroupTests, processesChannelsInOrder) {
		assignStubs(3);
		auto group = construct();
		buffer_type a{ 1 };
		buffer_type b{ 2, 3 };
		buffer_type c{ 4, 5, 6 };
		channels = { a, b, c };
		group.process(channels);
		assertEqual({ 1 }, processors.at(0)->processed());
		assertEqual({ 2, 3 }, processors.at(1)->processed());
		assertEqual({ 4, 5, 6 }, processors.at(2)->processed());
	}

	TEST_F(ChannelProcessingGroupTests, groupDelayReturnsMaxGroupDelay) {
		assignStubs(3);
		processors.at(0)->setGroupDelay(1);
		processors.at(1)->setGroupDelay(2);
		processors.at(2)->setGroupDelay(3);
		auto group = construct();
		assertEqual(3, group.groupDelay());
	}

	TEST_F(ChannelProcessingGroupTests, groupDelayReturnsZeroWhenNoProcessors) {
		auto group = construct();
		assertEqual(0, group.groupDelay());
	}

	TEST_F(ChannelProcessingGroupTests, processIgnoresExtraChannels) {
		assignStubs(2);
		auto group = construct();
		buffer_type a{ 1 };
		buffer_type b{ 2 };
		buffer_type c{ 3 };
		channels = { a, b, c };
		group.process(channels);
		assertEqual({ 1 }, processors.at(0)->processed());
		assertEqual({ 2 }, processors.at(1)->processed());
	}

	TEST_F(ChannelProcessingGroupTests, processOnlyChannelsAvailable) {
		assignStubs(3);
		auto group = construct();
		buffer_type a{ 1 };
		buffer_type b{ 2 };
		channels = { a, b };
		group.process(channels);
		assertEqual({ 1 }, processors.at(0)->processed());
		assertEqual({ 2 }, processors.at(1)->processed());
	}
}