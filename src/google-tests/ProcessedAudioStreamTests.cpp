#include <audio-stream-processing/ProcessedAudioStream.h>
#include <gtest/gtest.h>

class ReadsAOne : public AudioReader {
	void read(float ** channels, int) override {
		*channels[0] = 1;
	}
};

class AudioTimesTwo : public AudioProcessor {
	void process(float ** channels, int) override {
		*channels[0] *= 2;
	}
};

class ProcessedAudioStreamTestCase : public ::testing::TestCase {};

TEST(ProcessedAudioStreamTestCase, fillBufferReadsThenProcesses) {
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	ProcessedAudioStream stream{ reader, processor };
	float x{};
	float *channels[] = { &x };
	stream.fillBuffer(channels, 0);
	EXPECT_EQ(2, x);
}

TEST(ProcessedAudioStreamTestCase, fillBufferPassesParametersToReaderAndProcessor) {
	const auto reader = std::make_shared<MockAudioReader>();
	const auto processor = std::make_shared<MockAudioProcessor>();
	ProcessedAudioStream stream{ reader, processor };
	float *x;
	stream.fillBuffer(&x, 1);
	EXPECT_EQ(&x, reader->channels());
	EXPECT_EQ(1, reader->frameCount());
	EXPECT_EQ(&x, processor->channels());
	EXPECT_EQ(1, processor->frameCount());
}