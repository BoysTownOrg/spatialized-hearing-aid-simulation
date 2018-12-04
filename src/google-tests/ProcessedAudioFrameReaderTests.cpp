#include "AudioFrameReaderStub.h"
#include <audio-stream-processing/ProcessedAudioFrameReader.h>
#include <gtest/gtest.h>

class AudioFrameProcessorStub : public AudioFrameProcessor {
	int _frameCount{};
	float **_channels{};
public:
	int frameCount() const {
		return _frameCount;
	}

	const float *const *channels() const {
		return _channels;
	}

	void process(float ** channels, int frameCount) override {
		_channels = channels;
		_frameCount = frameCount;
	}
};

class ReadsAOne : public AudioFrameReader {
	void read(float ** channels, int) override {
		*channels[0] = 1;
	}
	bool complete() const override
	{
		return false;
	}
};

class AudioTimesTwo : public AudioFrameProcessor {
	void process(float ** channels, int) override {
		*channels[0] *= 2;
	}
};

class ProcessedAudioFrameReaderTestCase : public ::testing::TestCase {};

TEST(ProcessedAudioFrameReaderTestCase, fillBufferReadsThenProcesses) {
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	ProcessedAudioFrameReader stream{ reader, processor };
	float x{};
	float *channels[] = { &x };
	stream.read(channels, 0);
	EXPECT_EQ(2, x);
}

TEST(ProcessedAudioFrameReaderTestCase, fillBufferPassesParametersToReaderAndProcessor) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	const auto processor = std::make_shared<AudioFrameProcessorStub>();
	ProcessedAudioFrameReader stream{ reader, processor };
	float *x;
	stream.read(&x, 1);
	EXPECT_EQ(&x, reader->channels());
	EXPECT_EQ(1, reader->frameCount());
	EXPECT_EQ(&x, processor->channels());
	EXPECT_EQ(1, processor->frameCount());
}

TEST(ProcessedAudioFrameReaderTestCase, returnsCompleteWhenComplete) {
	const auto reader = std::make_shared<AudioFrameReaderStub>();
	ProcessedAudioFrameReader adapter{ reader, std::make_shared<AudioFrameProcessorStub>() };
	EXPECT_FALSE(adapter.complete());
	reader->setComplete();
	EXPECT_TRUE(adapter.complete());
}
