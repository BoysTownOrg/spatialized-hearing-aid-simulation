#include <playing-audio/AudioFrameProcessor.h>
#include <audio-stream-processing/AudioFrameReader.h>

class RefactoredAudioFrameProcessorImpl {
	AudioFrameReader *reader;
	AudioFrameProcessor *processor;
public:
	RefactoredAudioFrameProcessorImpl(
		AudioFrameReader *reader, 
		AudioFrameProcessor *processor
	) :
		reader{ reader },
		processor{ processor } {}

	void process(gsl::span<gsl::span<float>> audio) {
		processor->process(audio);
		reader->read(audio);
	}
};

class RefactoredAudioFrameProcessorImplFactory{};

#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include <gtest/gtest.h>

TEST(RefactoredAudioFrameProcessorTests, tbd) {
	AudioFrameReaderStub reader{};
	AudioFrameProcessorStub processor{};
	RefactoredAudioFrameProcessorImpl impl{&reader, &processor};
	gsl::span<float> x{};
	impl.process({ &x, 1 });
	EXPECT_EQ(&x, reader.audioBuffer().data());
	EXPECT_EQ(1, reader.audioBuffer().size());
	EXPECT_EQ(&x, processor.audioBuffer().data());
	EXPECT_EQ(1, processor.audioBuffer().size());
}

TEST(RefactoredAudioFrameProcessorTests, tbd2) {
	RefactoredAudioFrameProcessorImplFactory factory{};
}