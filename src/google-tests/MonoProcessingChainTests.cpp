#include "MockSignalProcessor.h"
#include <audio-processing/MonoProcessingChain.h>
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

class MonoProcessingChainTestCase : public ::testing::TestCase {};

TEST(MonoProcessingChainTestCase, chainCallsProcessorsInOrder) {
	MonoProcessingChain chain{};
	chain.add(std::make_shared<AddOne>());
	chain.add(std::make_shared<TimesTwo>());
	float x = 1;
	chain.process(&x, 0);
	EXPECT_EQ(4, x);
}

TEST(MonoProcessingChainTestCase, chainPassesParametersToProcessor) {
	MonoProcessingChain chain{};
	const auto processor = std::make_shared<MockSignalProcessor>();
	chain.add(processor);
	float x{};
	chain.process(&x, 1);
	EXPECT_EQ(&x, processor->signal());
	EXPECT_EQ(1, processor->frames());
}
