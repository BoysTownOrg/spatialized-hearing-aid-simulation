#include <gtest/gtest.h>

class HearingAidProcessorTestCase : public ::testing::TestCase {};

TEST(
	HearingAidProcessorTestCase, 
	processCallsFilterbankCompressorMethodsInCorrectOrder) 
{
	const auto compressor = std::make_shared<MockFilterbankCompressor>();
	HearingAidProcessor processor{ compressor };
	processor.process();
	EXPECT_EQ(
		"compressInput "
		"analyzeFilterbank "
		"compressChannels "
		"synthesizeFilterbank "
		"compressOutput ",
		compressor->processingLog());
}