#include <playing-audio/AudioFrameProcessor.h>
#include <audio-stream-processing/AudioFrameReader.h>

class RefactoredAudioFrameProcessor {
public:
	RefactoredAudioFrameProcessor(AudioFrameReader *, AudioFrameProcessor *) {}
};

#include "AudioFrameReaderStub.h"
#include "AudioFrameProcessorStub.h"
#include <gtest/gtest.h>

TEST(RefactoredAudioFrameProcessorTests, tbd) {
	AudioFrameReaderStub reader{};
	AudioFrameProcessorStub processor{};
	RefactoredAudioFrameProcessor{&reader, &processor};
}