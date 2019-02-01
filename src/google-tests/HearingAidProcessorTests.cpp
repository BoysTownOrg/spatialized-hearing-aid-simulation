#include "assert-utility.h"
#include "FilterbankCompressorSpy.h"
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <gtest/gtest.h>

namespace {
	class HearingAidProcessorTests : public ::testing::Test {
	protected:
		using signal_type = HearingAidProcessor::signal_type;
		using buffer_type = std::vector<signal_type::element_type>;
		std::shared_ptr<FilterbankCompressorSpy> compressor =
			compressorWithValidDefaults();
		HearingAidProcessor processor{ compressor };

	private:
		static std::shared_ptr<FilterbankCompressorSpy> compressorWithValidDefaults() {
			auto c = std::make_shared<FilterbankCompressorSpy>();
			c->setChunkSize(1);
			c->setWindowSize(1);
			return c;
		}

	protected:
		void processUnequalChunk() {
			buffer_type x(compressor->chunkSize() + 1);
			process_(x);
		}

		void process() {
			buffer_type x(compressor->chunkSize());
			process_(x);
		}

	private:
		void process_(signal_type x) {
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

	class CompressorErrorTests : public ::testing::Test {
	protected:
		std::shared_ptr<FilterbankCompressorSpy> compressor =
			std::make_shared<FilterbankCompressorSpy>();

		void assertConstructorThrowsCompressorError(std::string what) {
			try {
				HearingAidProcessor processor{ compressor };
				FAIL() << "Expected HearingAidProcessor::CompressorError";
			}
			catch (const HearingAidProcessor::CompressorError &e) {
				assertEqual(std::move(what), e.what());
			}
		}
	};

	TEST_F(
		CompressorErrorTests,
		failedCompressorThrowsCompressorError
	) {
		compressor->fail();
		assertConstructorThrowsCompressorError("The compressor failed to initialize.");
	}

	TEST_F(
		CompressorErrorTests,
		nonPowerOfTwoChunkSizeThrowsCompressorError
	) {
		compressor->setChunkSize(1023);
		assertConstructorThrowsCompressorError("The chunk size must be a power of two.");
	}

	TEST_F(
		CompressorErrorTests,
		nonPowerOfTwoWindowSizeThrowsCompressorError
	) {
		compressor->setWindowSize(1023);
		assertConstructorThrowsCompressorError("The window size must be a power of two.");
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

		bool failed() override { return false; }
		int chunkSize() override { return 1; }
		int channels() override { return 1; }
		int windowSize() override { return 1; }
		void compressChannels(complex_type *, complex_type *, int) override {}
	};

	TEST(
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

		int chunkSize() override {
			return chunkSize_;
		}

		int channels() override {
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

		bool failed() override { return false; }
		int windowSize() override { return 1; }
		void compressInput(real_type *, real_type *, int) override {}
		void compressOutput(real_type *, real_type *, int) override {}
	};

	class HearingAidProcessorComplexSignalTests : public ::testing::Test {
	protected:
		std::shared_ptr<ForComplexSignalTests> compressor =
			std::make_shared<ForComplexSignalTests>();

		void process() {
			HearingAidProcessor processor{ compressor };
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

	TEST_F(
		HearingAidProcessorComplexSignalTests,
		complexInputSizeIsAtLeastChannelTimesChunkSizeTimesTwo
	) {
		compressor->setChunkSize(4);
		compressor->setChannels(5);
		compressor->setPointerOffset(4 * 5 * 2 - 1);
		process();
		EXPECT_EQ(
			(0 + 7) * 11 * 13 * 17 * 19,
			compressor->postSynthesizeFilterbankComplexResult()
		);
	}
}