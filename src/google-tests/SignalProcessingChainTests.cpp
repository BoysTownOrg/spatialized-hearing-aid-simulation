#include "SignalProcessorStub.h"
#include <signal-processing/SignalProcessingChain.h>
#include <gtest/gtest.h>

class AddOne : public SignalProcessor {
public:
	void process(gsl::span<float> signal) override {
		for (auto &x : signal)
			x += 1;
	}
};

class TimesTwo : public SignalProcessor {
public:
	void process(gsl::span<float> signal) override {
		for (auto &x : signal)
			x *= 2;
	}
};

class SignalProcessingChainTestCase : public ::testing::TestCase {};

TEST(SignalProcessingChainTestCase, chainCallsProcessorsInOrder) {
	SignalProcessingChain chain{};
	chain.add(std::make_shared<AddOne>());
	chain.add(std::make_shared<TimesTwo>());
	float x = 1;
	chain.process({ &x, 1 });
	EXPECT_EQ(4, x);
}

TEST(SignalProcessingChainTestCase, chainPassesParametersToProcessor) {
	SignalProcessingChain chain{};
	const auto processor = std::make_shared<SignalProcessorStub>();
	chain.add(processor);
	float x{};
	chain.process({ &x, 1 });
	EXPECT_EQ(&x, processor->signal());
	EXPECT_EQ(1, processor->samples());
}
