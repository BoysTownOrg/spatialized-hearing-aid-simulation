#include <playing-audio/AudioFrameProcessor.h>
#include <audio-stream-processing/AudioFrameReader.h>

class RefactoredAudioFrameProcessorImpl {
	AudioFrameReader *reader;
	AudioFrameProcessor *processor;
	int paddedZeroes{};
public:
	RefactoredAudioFrameProcessorImpl(
		AudioFrameReader *reader, 
		AudioFrameProcessor *processor
	) :
		reader{ reader },
		processor{ processor } {}

	void process(gsl::span<gsl::span<float>> audio) {
		if (reader->complete()) {
			for (auto channel : audio)
				for (auto &x : channel) {
					++paddedZeroes;
					x = 0;
				}
		}
		reader->read(audio);
		processor->process(audio);
	}

	bool complete() {
		return paddedZeroes >= processor->groupDelay();
	}
};

class RefactoredAudioFrameProcessorImplFactory{};

#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include <gtest/gtest.h>

namespace {
	class RefactoredAudioFrameProcessorImplTests : public ::testing::Test {
	protected:
		AudioFrameReaderStub reader{};
		AudioFrameProcessorStub processor{};
		RefactoredAudioFrameProcessorImpl impl{ &reader, &processor };
	};

	TEST_F(RefactoredAudioFrameProcessorImplTests, processPassesAudioToReaderAndProcessor) {
		gsl::span<float> x{};
		impl.process({ &x, 1 });
		EXPECT_EQ(&x, reader.audioBuffer().data());
		EXPECT_EQ(1, reader.audioBuffer().size());
		EXPECT_EQ(&x, processor.audioBuffer().data());
		EXPECT_EQ(1, processor.audioBuffer().size());
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, processPadsZeroToEndOfInput) {
		reader.setComplete();
		std::vector<float> audio(3, -1);
		gsl::span<float> x{ audio };
		impl.process({ &x, 1 });
		EXPECT_EQ(0, processor.audioBuffer()[0][0]);
		EXPECT_EQ(0, processor.audioBuffer()[0][1]);
		EXPECT_EQ(0, processor.audioBuffer()[0][2]);
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, completeAfterProcessingPaddedZeroes) {
		reader.setComplete();
		processor.setGroupDelay(3);
		float y{};
		gsl::span<float> x{ &y, 1 };
		impl.process({ &x, 1 });
		EXPECT_FALSE(impl.complete());
		impl.process({ &x, 1 });
		EXPECT_FALSE(impl.complete());
		impl.process({ &x, 1 });
		EXPECT_TRUE(impl.complete());
	}

	TEST_F(RefactoredAudioFrameProcessorImplTests, initializePassesParametersToFactory) {
		RefactoredAudioFrameProcessorImpl::Initialization initialization;
		initialization.leftDslPrescriptionFilePath = "a";
		initialization.rightDslPrescriptionFilePath = "b";
		initialization.brirFilePath = "c";
		initialization.max_dB_Spl = 1;
		initialization.attack_ms = 2;
		initialization.release_ms = 3;
		initialization.windowSize = 4;
		initialization.chunkSize = 5;
		impl.initialize(initialization);
		assertEqual("a", processorFactory.parameters().leftDslPrescriptionFilePath);
		assertEqual("b", processorFactory.parameters().rightDslPrescriptionFilePath);
		assertEqual("c", processorFactory.parameters().brirFilePath);
		EXPECT_EQ(1, processorFactory.parameters().max_dB_Spl);
		EXPECT_EQ(2, processorFactory.parameters().attack_ms);
		EXPECT_EQ(3, processorFactory.parameters().release_ms);
		EXPECT_EQ(4, processorFactory.parameters().windowSize);
		EXPECT_EQ(5, processorFactory.parameters().chunkSize);
	}

	class ReadsAOne : public AudioFrameReader {
		void read(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x = 1;
		}

		int channels() const override {
			return 1;
		}

		bool complete() const override { return {}; }
		int sampleRate() const override { return {}; }
		long long frames() const override { return {}; }
		void reset() override {}
	};

	class TimesTwo : public AudioFrameProcessor {
		void process(gsl::span<gsl::span<float>> audio) override {
			for (const auto channel : audio)
				for (auto &x : channel)
					x *= 2;
		}

		int groupDelay() override { return {}; }
	};

	TEST(RefactoredAudioFrameProcessorOtherTests, processReadsThenProcesses) {
		ReadsAOne reader{};
		TimesTwo processor{};
		RefactoredAudioFrameProcessorImpl impl{ &reader, &processor };
		float y{};
		gsl::span<float> x{ &y, 1 };
		impl.process({ &x, 1 });
		EXPECT_EQ(2, y);
	}

	TEST(RefactoredAudioFrameProcessorOtherTests, tbd2) {
		RefactoredAudioFrameProcessorImplFactory factory{};
	}
}