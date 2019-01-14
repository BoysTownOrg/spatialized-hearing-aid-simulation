#include "assert-utility.h"
#include "FilterbankCompressorSpy.h"
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <gtest/gtest.h>

class HearingAidProcessorTests : public ::testing::Test {
protected:
	std::shared_ptr<FilterbankCompressorSpy> compressor =
		std::make_shared<FilterbankCompressorSpy>();
	HearingAidProcessor processor{ compressor };

	void processUnequalChunk() {
		std::vector<float> x(compressor->chunkSize() + 1);
		_process(x);
	}

	void process() {
		std::vector<float> x(compressor->chunkSize());
		_process(x);
	}

private:
	void _process(gsl::span<float> x) {
		processor.process(x);
	}
};

TEST_F(
	HearingAidProcessorTests, 
	processCallsFilterbankCompressorMethodsInCorrectOrder
) {
	process();
	assertEqual(
		"compressInput"
		"analyzeFilterbank"
		"compressChannels"
		"synthesizeFilterbank"
		"compressOutput",
		compressor->processingLog()
	);
}

TEST_F(
	HearingAidProcessorTests,
	processDoesNotCallCompressorWhenFrameCountDoesNotEqualChunkSize
) {
	processUnequalChunk();
	EXPECT_TRUE(compressor->processingLog().empty());
}

TEST_F(HearingAidProcessorTests, processPassesChunkSize) {
	compressor->setChunkSize(1);
	process();
	EXPECT_EQ(1, compressor->compressInputChunkSize());
	EXPECT_EQ(1, compressor->filterbankAnalyzeChunkSize());
	EXPECT_EQ(1, compressor->compressChannelsChunkSize());
	EXPECT_EQ(1, compressor->filterbankSynthesizeChunkSize());
	EXPECT_EQ(1, compressor->compressOutputChunkSize());
}

TEST(
	HearingAidProcessorOtherTests,
	failedCompressorThrowsCompressorError
) {
	try {
		const auto compressor = std::make_shared<FilterbankCompressorSpy>();
		compressor->fail();
		HearingAidProcessor processor{ compressor };
		FAIL() << "Expected HearingAidProcessor::CompressorError";
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		assertEqual("The compressor failed to initialize.", e.what());
	}
}

class MultipliesRealSignalsByPrimes : public FilterbankCompressor {
public:
	void compressInput(real_type *input, real_type *output, int) override {
		*input *= 2;
		*output *= 3;
	}

	void analyzeFilterbank(real_type *input, complex_type *, int) override {
		*input *= 5;
	}

	void compressChannels(complex_type *, complex_type *, int) override {}

	void synthesizeFilterbank(complex_type *, real_type *output, int) override {
		*output *= 7;
	}

	void compressOutput(real_type *input, real_type *output, int) override {
		*input *= 11;
		*output *= 13;
	}

	int chunkSize() const override {
		return 1;
	}

	int channels() const override {
		return 1;
	}

	bool failed() const override {
		return false;
	}
};

TEST(
	HearingAidProcessorOtherTests,
	processPassesRealInputsAppropriately
) {
	const auto compressor = std::make_shared<MultipliesRealSignalsByPrimes>();
	HearingAidProcessor processor{ compressor };
	std::vector<float> x = { 4 };
	processor.process(x);
	assertEqual({ 4 * 2 * 3 * 5 * 7 * 11 * 13 }, x);
}

class ForComplexSignalTests : public FilterbankCompressor {
	int _chunkSize{ 1 };
	int _channels{ 1 };
	int _pointerOffset{};
	complex_type _postSynthesizeFilterbankComplexResult{};
public:
	void compressInput(real_type *, real_type *, int) override {}

	void analyzeFilterbank(real_type *, complex_type *output, int) override {
		*(output + _pointerOffset) += 7;
		*(output + _pointerOffset) *= 11;
	}

	void compressChannels(complex_type *input, complex_type *output, int) override {
		*(input + _pointerOffset) *= 13;
		*(output + _pointerOffset) *= 17;
	}

	void synthesizeFilterbank(complex_type *input, real_type *, int) override {
		*(input + _pointerOffset) *= 19;
		_postSynthesizeFilterbankComplexResult = *(input + _pointerOffset);
	}

	void compressOutput(real_type *, real_type *, int) override {}

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

	void setPointerOffset(int offset) {
		_pointerOffset = offset;
	}

	complex_type postSynthesizeFilterbankComplexResult() const {
		return _postSynthesizeFilterbankComplexResult;
	}

	bool failed() const override {
		return false;
	}
};

class HearingAidProcessorFacade {
	HearingAidProcessor processor;
	std::shared_ptr<FilterbankCompressor> compressor;
public:
	explicit HearingAidProcessorFacade(
		std::shared_ptr<FilterbankCompressor> compressor
	) :
		processor{ compressor },
		compressor{ compressor } {}

	void processUnequalChunk() {
		std::vector<float> x(compressor->chunkSize() + 1);
		_process(x);
	}

	void process() {
		std::vector<float> x(compressor->chunkSize());
		_process(x);
	}

	void process(gsl::span<float> x) {
		_process(x);
	}

private:
	void _process(gsl::span<float> x) {
		processor.process(x);
	}
};

TEST(
	HearingAidProcessorOtherTests,
	processPassesComplexInputsAppropriately
) {
	const auto compressor = std::make_shared<ForComplexSignalTests>();
	HearingAidProcessorFacade processor{ compressor };
	processor.process();
	EXPECT_EQ(
		(0 + 7) * 11 * 13 * 17 * 19,
		compressor->postSynthesizeFilterbankComplexResult()
	);
}

TEST(
	HearingAidProcessorOtherTests,
	complexInputSizeIsAtLeastChannelTimesChunkSizeTimesTwo
) {
	const auto compressor = std::make_shared<ForComplexSignalTests>();
	compressor->setChunkSize(3);
	compressor->setChannels(5);
	compressor->setPointerOffset(3 * 5 * 2 - 1);
	HearingAidProcessorFacade processor{ compressor };
	processor.process();
	EXPECT_EQ(
		(0 + 7) * 11 * 13 * 17 * 19,
		compressor->postSynthesizeFilterbankComplexResult()
	);
}