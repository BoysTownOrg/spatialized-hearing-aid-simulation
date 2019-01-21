#include "SignalProcessorStub.h"
#include <signal-processing/SignalProcessingChain.h>
#include <gtest/gtest.h>

namespace {
	class AddOne : public SignalProcessor {
	public:
		void process(signal_type signal) override {
			for (auto &x : signal)
				x += 1;
		}

		index_type groupDelay() override { return {}; }
	};

	class TimesTwo : public SignalProcessor {
	public:
		void process(signal_type signal) override {
			for (auto &x : signal)
				x *= 2;
		}

		index_type groupDelay() override { return {}; }
	};

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
		float x = 1;
		chain.process({ &x, 1 });
		EXPECT_EQ(4, x);
	}

	TEST_F(SignalProcessingChainTests, chainPassesParametersToProcessor) {
		float x{};
		const gsl::span<float> channel{ &x, 1 };
		chain.process(channel);
		EXPECT_EQ(channel, processor->signal());
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
