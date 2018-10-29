#include <audio-processing/MonoProcessor.h>
#include <memory>
#include <vector>

class MonoProcessingChain : public MonoProcessor {
	std::vector<std::shared_ptr<MonoProcessor>> processors;
public:
	void process(float *x, int) override {
		for (const auto &processor : processors)
			processor->process(x, 0);
	}
	void add(std::shared_ptr<MonoProcessor> processor) {
		processors.push_back(processor);
	}
};

#include "MockMonoProcessor.h"
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

TEST(MonoProcessingChainTestCase, tbd) {
	const auto first = std::make_shared<AddOne>();
	const auto second = std::make_shared<TimesTwo>();
	MonoProcessingChain chain{};
	chain.add(first);
	chain.add(second);
	float x = 1;
	chain.process(&x, 0);
	EXPECT_EQ(4, x);
}