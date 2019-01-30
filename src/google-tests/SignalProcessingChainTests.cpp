#include "assert-utility.h"
#include "SignalProcessorStub.h"
#include <signal-processing/SignalProcessingChain.h>
#include <gtest/gtest.h>

namespace {
	class SignalProcessingChainTests : public ::testing::Test {
	protected:
		SignalProcessingChain chain{};
		std::shared_ptr<SignalProcessorStub> processor =
			std::make_shared<SignalProcessorStub>();

		SignalProcessingChainTests() {
			chain.add(processor);
		}
	};

	TEST_F(SignalProcessingChainTests, chainCallsProcessorsInOrder) {
		chain.add(std::make_shared<AddOne>());
		chain.add(std::make_shared<TimesTwo>());
		std::vector<float> x = { 1, 2, 3 };
		chain.process(x);
		assertEqual({ 4, 6, 8 }, x);
	}

	TEST_F(SignalProcessingChainTests, groupDelayReturnsSumOfComponents) {
		processor->setGroupDelay(1);
		auto processor2 = std::make_shared<SignalProcessorStub>();
		processor2->setGroupDelay(2);
		chain.add(processor2);
		auto processor3 = std::make_shared<SignalProcessorStub>();
		processor3->setGroupDelay(3);
		chain.add(processor3);
		EXPECT_EQ(1 + 2 + 3, chain.groupDelay());
	}
}
