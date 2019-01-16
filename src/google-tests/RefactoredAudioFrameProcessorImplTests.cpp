#include <playing-audio/AudioFrameProcessor.h>
#include <audio-stream-processing/AudioFrameReader.h>

class RefactoredAudioFrameProcessorImpl {
public:
	RefactoredAudioFrameProcessorImpl(AudioFrameReader *, AudioFrameProcessor *) {}
};

class RefactoredAudioFrameProcessorImplFactory{};

#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include <gtest/gtest.h>

TEST(RefactoredAudioFrameProcessorTests, tbd) {
	AudioFrameReaderStub reader{};
	AudioFrameProcessorStub processor{};
	RefactoredAudioFrameProcessorImpl impl{&reader, &processor};
	gsl::span<gsl::span<float>> x{};
	impl.process(x);
	EXPECT_EQ(x, reader.audioBuffer());
	EXPECT_EQ(x, processor.audioBuffer());
}

TEST(RefactoredAudioFrameProcessorTests, tbd2) {
	RefactoredAudioFrameProcessorImplFactory factory{};
}