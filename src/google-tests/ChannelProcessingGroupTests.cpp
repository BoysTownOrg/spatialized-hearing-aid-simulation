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
		buffer_type b{ 2 };
		buffer_type c{ 3 };
		std::vector<channel_type> channels{ a, b, c };
		group.process(channels);
		assertEqual(1.0f, processors.at(0)->signal().at(0));
		assertEqual(2.0f, processors.at(1)->signal().at(0));
		assertEqual(3.0f, processors.at(2)->signal().at(0));
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

	TEST_F(ChannelProcessingGroupTests, processIncorrectNumberOfChannelsJustReturns) {
		assignStubs(2);
		auto group = construct();
		buffer_type a{ 1 };
		buffer_type b{ 2 };
		buffer_type c{ 3 };
		std::vector<channel_type> channels{ a, b, c };
		group.process(channels);
	}
}