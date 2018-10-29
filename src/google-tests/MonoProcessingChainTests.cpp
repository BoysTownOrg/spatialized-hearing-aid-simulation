#include <audio-processing/MonoProcessingChain.h>
#include <gtest/gtest.h>

class AddOne : public MonoProcessor {
public:
	void process(float *x, int) override {
		*x += 1;
	}
};

class TimesTwo : public MonoProcessor {
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