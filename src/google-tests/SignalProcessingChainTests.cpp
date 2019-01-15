#include "SignalProcessorStub.h"
#include <signal-processing/SignalProcessingChain.h>
#include <gtest/gtest.h>

namespace {
	class AddOne : public SignalProcessor {
	public:
		void process(gsl::span<float> signal) override {
			for (auto &x : signal)
				x += 1;
		}

		int groupDelay() override { return {}; }
	};

	class TimesTwo : public SignalProcessor {
	public:
		void process(gsl::span<float> signal) override {
			for (auto &x : signal)
				x *= 2;
		}
		
		int groupDelay() override { return {}; }
	};
}

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
	gsl::span<float> channel{ &x, 1 };
	chain.process(channel);
	EXPECT_EQ(channel, processor->signal());
}
