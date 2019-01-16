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
	float x{ 5 };
	gsl::span<float> y{ &x, 1 };
	gsl::span<gsl::span<float>> z{ &y, 1 };
	impl.process(z);
	EXPECT_EQ(5, reader.audioBuffer().at(0).at(0));
	EXPECT_EQ(5, processor.audioBuffer().at(0).at(0));
}

TEST(RefactoredAudioFrameProcessorTests, tbd2) {
	RefactoredAudioFrameProcessorImplFactory factory{};
}