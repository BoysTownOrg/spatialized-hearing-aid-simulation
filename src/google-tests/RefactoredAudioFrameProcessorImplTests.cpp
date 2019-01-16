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
		processor->process(audio);
		reader->read(audio);
	}

	bool complete() {
		return paddedZeroes >= processor->groupDelay();
	}
};

class RefactoredAudioFrameProcessorImplFactory{};

#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include <gtest/gtest.h>

class RefactoredAudioFrameProcessorImplTests : public ::testing::Test {
protected:
	AudioFrameReaderStub reader{};
	AudioFrameProcessorStub processor{};
	RefactoredAudioFrameProcessorImpl impl{&reader, &processor};
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

TEST(RefactoredAudioFrameProcessorOtherTests, tbd2) {
	RefactoredAudioFrameProcessorImplFactory factory{};
}