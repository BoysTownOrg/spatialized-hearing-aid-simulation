#include <playing-audio/AudioFrameProcessor.h>
#include <audio-stream-processing/AudioFrameReader.h>

class RefactoredAudioFrameProcessorImpl {
public:
	RefactoredAudioFrameProcessorImpl(AudioFrameReader *, AudioFrameProcessor *) {}
	void process(gsl::span<gsl::span<float>>) {}
};

class RefactoredAudioFrameProcessorImplFactory{};

#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include <gtest/gtest.h>

TEST(RefactoredAudioFrameProcessorTests, tbd) {
	AudioFrameReaderStub reader{};
	AudioFrameProcessorStub processor{};
	RefactoredAudioFrameProcessorImpl impl{&reader, &processor};
	std::vector<float> y{ 1, 2, 3 };
	gsl::span<gsl::span<float>> x{ { y } };
	impl.process(x);
	EXPECT_EQ(1, reader.audioBuffer().at(0).at(0));
	EXPECT_EQ(2, reader.audioBuffer().at(0).at(1));
	EXPECT_EQ(3, reader.audioBuffer().at(0).at(2));
	EXPECT_EQ(1, processor.audioBuffer().at(0).at(0));
	EXPECT_EQ(2, processor.audioBuffer().at(0).at(1));
	EXPECT_EQ(3, processor.audioBuffer().at(0).at(2));
}

TEST(RefactoredAudioFrameProcessorTests, tbd2) {
	RefactoredAudioFrameProcessorImplFactory factory{};
}