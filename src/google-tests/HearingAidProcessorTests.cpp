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
		process_(x);
	}

	void process() {
		std::vector<float> x(compressor->chunkSize());
		process_(x);
	}

private:
	void process_(gsl::span<float> x) {
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

TEST_F(
	HearingAidProcessorTests,
	groupDelayReturnsHalfWindowSize
) {
	compressor->setWindowSize(512);
	EXPECT_EQ(256, processor.groupDelay());
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

TEST(
	HearingAidProcessorOtherTests,
	nonPowerOfTwoChunkSizeThrowsCompressorError
) {
	try {
		const auto compressor = std::make_shared<FilterbankCompressorSpy>();
		compressor->setChunkSize(1023);
		HearingAidProcessor processor{ compressor };
		FAIL() << "Expected HearingAidProcessor::CompressorError";
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		assertEqual("The chunk size must be a power of two.", e.what());
	}
}

TEST(
	HearingAidProcessorOtherTests,
	nonPowerOfTwoWindowSizeThrowsCompressorError
) {
	try {
		const auto compressor = std::make_shared<FilterbankCompressorSpy>();
		compressor->setWindowSize(1023);
		HearingAidProcessor processor{ compressor };
		FAIL() << "Expected HearingAidProcessor::CompressorError";
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		assertEqual("The window size must be a power of two.", e.what());
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

	int windowSize() const override { return {}; }
	void compressChannels(complex_type *, complex_type *, int) override {}
};

class HearingAidProcessorRealSignalTests : public ::testing::Test {};

TEST_F(
	HearingAidProcessorRealSignalTests,
	processPassesRealInputsAppropriately
) {
	HearingAidProcessor processor{ std::make_shared<MultipliesRealSignalsByPrimes>() };
	std::vector<float> x = { 4 };
	processor.process(x);
	assertEqual({ 4 * 2 * 3 * 5 * 7 * 11 * 13 }, x);
}

class ForComplexSignalTests : public FilterbankCompressor {
	complex_type postSynthesizeFilterbankComplexResult_{};
	int chunkSize_{ 1 };
	int channels_{ 1 };
	int pointerOffset_{};
public:
	void analyzeFilterbank(real_type *, complex_type *output, int) override {
		*(output + pointerOffset_) += 7;
		*(output + pointerOffset_) *= 11;
	}

	void compressChannels(complex_type *input, complex_type *output, int) override {
		*(input + pointerOffset_) *= 13;
		*(output + pointerOffset_) *= 17;
	}

	void synthesizeFilterbank(complex_type *input, real_type *, int) override {
		*(input + pointerOffset_) *= 19;
		postSynthesizeFilterbankComplexResult_ = *(input + pointerOffset_);
	}

	int chunkSize() const override {
		return chunkSize_;
	}

	int channels() const override {
		return channels_;
	}

	void setChunkSize(int s) {
		chunkSize_ = s;
	}

	void setChannels(int c) {
		channels_ = c;
	}

	void setPointerOffset(int offset) {
		pointerOffset_ = offset;
	}

	complex_type postSynthesizeFilterbankComplexResult() const {
		return postSynthesizeFilterbankComplexResult_;
	}

	bool failed() const override {
		return false;
	}

	int windowSize() const override { return {}; }
	void compressInput(real_type *, real_type *, int) override {}
	void compressOutput(real_type *, real_type *, int) override {}
};

class HearingAidProcessorComplexSignalTests : public ::testing::Test {
protected:
	std::shared_ptr<ForComplexSignalTests> compressor =
		std::make_shared<ForComplexSignalTests>();
	HearingAidProcessor processor{ compressor };

	void process() {
		std::vector<float> x(compressor->chunkSize());
		processor.process(x);
	}
};

TEST_F(
	HearingAidProcessorComplexSignalTests,
	processPassesComplexInputsAppropriately
) {
	process();
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
	compressor->setChunkSize(4);
	compressor->setChannels(5);
	compressor->setPointerOffset(4 * 5 * 2 - 1);
	HearingAidProcessor processor{ compressor };
	std::vector<float> x(compressor->chunkSize());
	processor.process(x);
	EXPECT_EQ(
		(0 + 7) * 11 * 13 * 17 * 19,
		compressor->postSynthesizeFilterbankComplexResult()
	);
}