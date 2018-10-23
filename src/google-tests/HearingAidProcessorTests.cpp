#include <common-includes/Interface.h>

class FilterbankCompressor {
public:
	INTERFACE_OPERATIONS(FilterbankCompressor);
	virtual void compressInput(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void analyzeFilterbank(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void compressChannels(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void synthesizeFilterbank(
		float *input,
		float *output,
		int chunkSize) = 0;
	virtual void compressOutput(
		float *input,
		float *output,
		int chunkSize) = 0;
};

#include <memory>

class HearingAidProcessor {
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit HearingAidProcessor(
		std::shared_ptr<FilterbankCompressor> compressor
	) :
		compressor{ std::move(compressor) } {}
	void process() {
		compressor->compressInput(nullptr, nullptr, 0);
		compressor->analyzeFilterbank(nullptr, nullptr, 0);
		compressor->compressChannels(nullptr, nullptr, 0);
		compressor->synthesizeFilterbank(nullptr, nullptr, 0);
		compressor->compressOutput(nullptr, nullptr, 0);
	}
};

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