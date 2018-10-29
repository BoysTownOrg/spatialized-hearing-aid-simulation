#include "MockSignalProcessor.h"
#include <audio-processing/SignalProcessingChain.h>
#include <gtest/gtest.h>

class AddOne : public SignalProcessor {
public:
	void process(float *x, int) override {
		*x += 1;
	}
};

class TimesTwo : public SignalProcessor {
public:
	void process(float *x, int) override {
		*x *= 2;
	}
};

class SignalProcessingChainTestCase : public ::testing::TestCase {};

TEST(SignalProcessingChainTestCase, chainCallsProcessorsInOrder) {
	SignalProcessingChain chain{};
	chain.add(std::make_shared<AddOne>());
	chain.add(std::make_shared<TimesTwo>());
	float x = 1;
	chain.process(&x, 0);
	EXPECT_EQ(4, x);
}

TEST(SignalProcessingChainTestCase, chainPassesParametersToProcessor) {
	SignalProcessingChain chain{};
	const auto processor = std::make_shared<MockSignalProcessor>();
	chain.add(processor);
	float x{};
	chain.process(&x, 1);
	EXPECT_EQ(&x, processor->signal());
	EXPECT_EQ(1, processor->frames());
}
