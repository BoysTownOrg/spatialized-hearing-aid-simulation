#include <audio-stream-processing/ProcessedAudioFrameReader.h>
#include <gtest/gtest.h>

class MockAudioReader : public AudioFrameReader {
	int _frameCount{};
	float **_channels{};
public:
	int frameCount() const {
		return _frameCount;
	}
	const float *const *channels() const {
		return _channels;
	}
	void read(float ** channels, int frameCount) override {
		_channels = channels;
		_frameCount = frameCount;
	}
};

class MockAudioProcessor : public AudioFrameProcessor {
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
};

class AudioTimesTwo : public AudioFrameProcessor {
	void process(float ** channels, int) override {
		*channels[0] *= 2;
	}
};

class ProcessedAudioStreamTestCase : public ::testing::TestCase {};

TEST(ProcessedAudioStreamTestCase, fillBufferReadsThenProcesses) {
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	ProcessedAudioFrameReader stream{ reader, processor };
	float x{};
	float *channels[] = { &x };
	stream.read(channels, 0);
	EXPECT_EQ(2, x);
}

TEST(ProcessedAudioStreamTestCase, fillBufferPassesParametersToReaderAndProcessor) {
	const auto reader = std::make_shared<MockAudioReader>();
	const auto processor = std::make_shared<MockAudioProcessor>();
	ProcessedAudioFrameReader stream{ reader, processor };
	float *x;
	stream.read(&x, 1);
	EXPECT_EQ(&x, reader->channels());
	EXPECT_EQ(1, reader->frameCount());
	EXPECT_EQ(&x, processor->channels());
	EXPECT_EQ(1, processor->frameCount());
}