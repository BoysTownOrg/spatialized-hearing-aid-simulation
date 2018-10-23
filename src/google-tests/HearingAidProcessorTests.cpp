#include <hearing-aid-processing/HearingAidProcessor.h>
#include <gtest/gtest.h>
#include <string>

class MockFilterbankCompressor : public FilterbankCompressor {
	std::string _processingLog{};
public:
	std::string processingLog() const {
		return _processingLog;
	}
	virtual void compressInput(float *, float *, int) override
	{
		_processingLog += "compressInput";
	}
	virtual void analyzeFilterbank(float *, float *, int) override
	{
		_processingLog += "analyzeFilterbank";
	}
	virtual void compressChannels(float *, float *, int) override
	{
		_processingLog += "compressChannels";
	}
	virtual void synthesizeFilterbank(float * , float * , int ) override
	{
		_processingLog += "synthesizeFilterbank";
	}
	virtual void compressOutput(float * , float * , int ) override
	{
		_processingLog += "compressOutput";
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
		"compressInput"
		"analyzeFilterbank"
		"compressChannels"
		"synthesizeFilterbank"
		"compressOutput",
		compressor->processingLog());
}