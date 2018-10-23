#include <hearing-aid-processing/HearingAidProcessor.h>
#include <gtest/gtest.h>
#include <string>

class MockFilterbankCompressor : public FilterbankCompressor {
	std::string _processingLog{};
	int _chunkSize{};
	int _compressInputChunkSize{};
	int _filterbankAnalyzeChunkSize{};
	int _compressChannelsChunkSize{};
	int _filterbankSynthesizeChunkSize{};
	int _compressOutputChunkSize{};
public:
	std::string processingLog() const {
		return _processingLog;
	}
	virtual void compressInput(float *, float *, int chunkSize) override
	{
		_compressInputChunkSize = chunkSize;
		_processingLog += "compressInput";
	}
	virtual void analyzeFilterbank(float *, float *, int chunkSize) override
	{
		_filterbankAnalyzeChunkSize = chunkSize;
		_processingLog += "analyzeFilterbank";
	}
	virtual void compressChannels(float *, float *, int chunkSize) override
	{
		_compressChannelsChunkSize = chunkSize;
		_processingLog += "compressChannels";
	}
	virtual void synthesizeFilterbank(float * , float * , int chunkSize) override
	{
		_filterbankSynthesizeChunkSize = chunkSize;
		_processingLog += "synthesizeFilterbank";
	}
	virtual void compressOutput(float * , float * , int chunkSize) override
	{
		_compressOutputChunkSize = chunkSize;
		_processingLog += "compressOutput";
	}
	void setChunkSize(int s) {
		_chunkSize = s;
	}
	int compressInputChunkSize() const {
		return _compressInputChunkSize;
	}
	int filterbankAnalyzeChunkSize() const {
		return _filterbankAnalyzeChunkSize;
	}
	int compressChannelsChunkSize() const {
		return _compressChannelsChunkSize;
	}
	int filterbankSynthesizeChunkSize() const {
		return _filterbankSynthesizeChunkSize;
	}
	int compressOutputChunkSize() const {
		return _compressOutputChunkSize;
	}
	int chunkSize() const override {
		return _chunkSize;
	}
};

class HearingAidProcessorTestCase : public ::testing::TestCase {};

TEST(
	HearingAidProcessorTestCase, 
	processCallsFilterbankCompressorMethodsInCorrectOrder) 
{
	const auto compressor = std::make_shared<MockFilterbankCompressor>();
	HearingAidProcessor processor{ compressor };
	processor.process(0);
	EXPECT_EQ(
		"compressInput"
		"analyzeFilterbank"
		"compressChannels"
		"synthesizeFilterbank"
		"compressOutput",
		compressor->processingLog());
}

TEST(HearingAidProcessorTestCase, processPassesChunkSize)
{
	const auto compressor = std::make_shared<MockFilterbankCompressor>();
	compressor->setChunkSize(1);
	HearingAidProcessor processor{ compressor };
	processor.process(1);
	EXPECT_EQ(1, compressor->compressInputChunkSize());
	EXPECT_EQ(1, compressor->filterbankAnalyzeChunkSize());
	EXPECT_EQ(1, compressor->compressChannelsChunkSize());
	EXPECT_EQ(1, compressor->filterbankSynthesizeChunkSize());
	EXPECT_EQ(1, compressor->compressOutputChunkSize());
}

TEST(
	HearingAidProcessorTestCase,
	processDoesNotCallCompressorWhenFrameCountDoesNotEqualChunkSize)
{
	const auto compressor = std::make_shared<MockFilterbankCompressor>();
	compressor->setChunkSize(1);
	HearingAidProcessor processor{ compressor };
	processor.process(2);
	EXPECT_TRUE(compressor->processingLog().empty());
}

class PrimeMultiplier : public FilterbankCompressor {
public:
	void compressInput(float *input, float *output, int) override {
		*input *= 2;
		*output *= 3;
	}
	void analyzeFilterbank(float *input, float *output, int) override {
		*input *= 5;
		*output *= 7;
	}
	void compressChannels(float *input, float *output, int) override {
		*input *= 11;
		*output *= 13;
	}
	void synthesizeFilterbank(float *input, float *output, int) override {
		*input *= 17;
		*output *= 19;
	}
	void compressOutput(float *input, float *output, int) override {
		*input *= 23;
		*output *= 29;
	}
	int chunkSize() const override {
		return {};
	}
};

TEST(
	HearingAidProcessorTestCase,
	processPassesInputAppropriately)
{
	const auto compressor = std::make_shared<PrimeMultiplier>();
	HearingAidProcessor processor{ compressor };
	std::vector<float> x = { 4 };
	processor.process(&x[0], 0);
	assertEqual({ 4 * 2 * 3 * 5 * 19 * 23 * 29 }, x);
}
