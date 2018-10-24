#include "assert-utility.h"
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <gtest/gtest.h>
#include <string>

class HearingAidProcessorFacade {
	HearingAidProcessor processor;
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit HearingAidProcessorFacade(
		std::shared_ptr<FilterbankCompressor> compressor
	) :
		processor{ compressor },
		compressor{ compressor } {}
	void processChunk() {
		processor.process(nullptr, compressor->chunkSize());
	}
	void processUnequalChunk() {
		processor.process(nullptr, compressor->chunkSize() + 1);
	}
	void processChunk(float *x) {
		processor.process(x, compressor->chunkSize());
	}
};

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
	virtual void compressInput(real *, real *, int chunkSize) override
	{
		_compressInputChunkSize = chunkSize;
		_processingLog += "compressInput";
	}
	virtual void analyzeFilterbank(real *, complex *, int chunkSize) override
	{
		_filterbankAnalyzeChunkSize = chunkSize;
		_processingLog += "analyzeFilterbank";
	}
	virtual void compressChannels(complex *, complex *, int chunkSize) override
	{
		_compressChannelsChunkSize = chunkSize;
		_processingLog += "compressChannels";
	}
	virtual void synthesizeFilterbank(complex * , real * , int chunkSize) override
	{
		_filterbankSynthesizeChunkSize = chunkSize;
		_processingLog += "synthesizeFilterbank";
	}
	virtual void compressOutput(real * , real * , int chunkSize) override
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
	int channels() const override {
		return {};
	}
};

class HearingAidProcessorTestCase : public ::testing::TestCase {};

TEST(
	HearingAidProcessorTestCase, 
	processCallsFilterbankCompressorMethodsInCorrectOrder) 
{
	const auto compressor = std::make_shared<MockFilterbankCompressor>();
	HearingAidProcessorFacade processor{ compressor };
	processor.processChunk();
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
	HearingAidProcessorFacade processor{ compressor };
	processor.processChunk();
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
	HearingAidProcessorFacade processor{ compressor };
	processor.processUnequalChunk();
	EXPECT_TRUE(compressor->processingLog().empty());
}

class RealSignalPrimeMultiplier : public FilterbankCompressor {
public:
	void compressInput(real *input, real *output, int) override {
		*input *= 2;
		*output *= 3;
	}
	void analyzeFilterbank(real *input, complex *, int) override {
		*input *= 5;
	}
	void compressChannels(complex *, complex *, int) override {
	}
	void synthesizeFilterbank(complex *, real *output, int) override {
		*output *= 7;
	}
	void compressOutput(real *input, real *output, int) override {
		*input *= 11;
		*output *= 13;
	}
	int chunkSize() const override {
		return {};
	}
	int channels() const override {
		return {};
	}
};

TEST(
	HearingAidProcessorTestCase,
	processPassesRealInputsAppropriately)
{
	const auto compressor = std::make_shared<RealSignalPrimeMultiplier>();
	HearingAidProcessorFacade processor{ compressor };
	std::vector<float> x = { 4 };
	processor.processChunk(&x[0]);
	assertEqual({ 4 * 2 * 3 * 5 * 7 * 11 * 13 }, x);
}

class ComplexSignalManipulator : public FilterbankCompressor {
	int _chunkSize{};
	int _channels{};
	complex _postSynthesizeFilterbankComplexResult{};
public:
	void compressInput(real *, real *, int) override {
	}
	void analyzeFilterbank(real *, complex *output, int) override {
		*output += 1;
		*output *= 2;
	}
	void compressChannels(complex *input, complex *output, int) override {
		*input *= 3;
		*output *= 5;
	}
	void synthesizeFilterbank(complex *input, real *, int) override {
		*input *= 7;
		_postSynthesizeFilterbankComplexResult = *input;
	}
	void compressOutput(real *, real *, int) override {
	}
	int chunkSize() const override {
		return _chunkSize;
	}
	int channels() const override {
		return _channels;
	}
	void setChunkSize(int s) {
		_chunkSize = s;
	}
	void setChannels(int c) {
		_channels = c;
	}
	complex postSynthesizeFilterbankComplexResult() const {
		return _postSynthesizeFilterbankComplexResult;
	}
};

TEST(
	HearingAidProcessorTestCase,
	processPassesComplexInputsAppropriately)
{
	const auto compressor = std::make_shared<ComplexSignalManipulator>();
	compressor->setChunkSize(1);
	compressor->setChannels(1);
	HearingAidProcessor processor{ compressor };
	processor.process(nullptr, 1);
	EXPECT_EQ(
		(0 + 1) * 2 * 3 * 5 * 7, 
		compressor->postSynthesizeFilterbankComplexResult());
}

class ComplexSignalBackManipulator : public FilterbankCompressor {
	int _chunkSize{};
	int _channels{};
	int _pointerOffset{};
	complex _postSynthesizeFilterbankComplexResult{};
public:
	void compressInput(real *, real *, int) override {
	}
	void analyzeFilterbank(real *, complex *output, int) override {
		*(output + _pointerOffset) += 7;
		*(output + _pointerOffset) *= 11;
	}
	void compressChannels(complex *input, complex *output, int) override {
		*(input + _pointerOffset) *= 13;
		*(output + _pointerOffset) *= 17;
	}
	void synthesizeFilterbank(complex *input, real *, int) override {
		*(input + _pointerOffset) *= 19;
		_postSynthesizeFilterbankComplexResult = *(input + _pointerOffset);
	}
	void compressOutput(real *, real *, int) override {
	}
	int chunkSize() const override {
		return _chunkSize;
	}
	void setChunkSize(int s) {
		_chunkSize = s;
	}
	void setChannels(int c) {
		_channels = c;
	}
	void setPointerOffset(int offset) {
		_pointerOffset = offset;
	}
	complex postSynthesizeFilterbankComplexResult() const {
		return _postSynthesizeFilterbankComplexResult;
	}
	int channels() const override {
		return _channels;
	}
};

TEST(
	HearingAidProcessorTestCase,
	complexInputSizeIsAtLeastChannelTimesChunkSizeTimesTwo)
{
	const auto compressor = std::make_shared<ComplexSignalBackManipulator>();
	compressor->setChunkSize(3);
	compressor->setChannels(5);
	compressor->setPointerOffset(3 * 5 * 2 - 1);
	HearingAidProcessor processor{ compressor };
	processor.process(nullptr, 3);
	EXPECT_EQ(
		(0 + 7) * 11 * 13 * 17 * 19,
		compressor->postSynthesizeFilterbankComplexResult());
}