#include <gtest/gtest.h>

class MonoProcessingChainTestCase : public ::testing::TestCase {};

TEST(MonoProcessingChainTestCase, tbd) {
	const auto first = std::make_shared<MockMonoProcessor>();
	const auto second = std::make_shared<MockMonoProcessor>();
	MonoProcessingChain chain{};
	chain.add(first);
	chain.add(second);
	chain.process(nullptr, 0);
	assertFirstCalledThenSecond();
}