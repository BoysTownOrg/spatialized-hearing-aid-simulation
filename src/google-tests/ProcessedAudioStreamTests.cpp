#include <gtest/gtest.h>

class ProcessedAudioStreamTestCase : public ::testing::TestCase {};

TEST(ProcessedAudioStreamTestCase, tbd) {
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<TimesTwo>();
	ProcessedAudioStream stream{ reader, processor };
	float x{};
	float *channels[] = { &x };
	stream.fillBuffer(channels, 0);
	EXPECT_EQ(2, x);
}