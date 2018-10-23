class FilterbankCompressor {};

#include <memory>

class HearingAidProcessor {
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit HearingAidProcessor(
		std::shared_ptr<FilterbankCompressor> compressor
	) :
		compressor{ std::move(compressor) } {}
	void process() {

	}
};

#include <gtest/gtest.h>

class MockFilterbankCompressor : public FilterbankCompressor {
	std::string _processingLog{};
public:
	std::string processingLog() const {
		return _processingLog;
	}
};

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