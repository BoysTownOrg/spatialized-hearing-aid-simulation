#include <common-includes/Interface.h>

class AudioReader {
public:
	INTERFACE_OPERATIONS(AudioReader);
	virtual void read(float **channels, int frameCount) = 0;
};

class AudioProcessor {
public:
	INTERFACE_OPERATIONS(AudioProcessor);
	virtual void process(float **channels, int frameCount) = 0;
};

#include <audio-device-control/AudioStream.h>
#include <memory>

class ProcessedAudioStream : public AudioStream {
	std::shared_ptr<AudioReader> reader;
	std::shared_ptr<AudioProcessor> processor;
public:
	ProcessedAudioStream(
		std::shared_ptr<AudioReader> reader,
		std::shared_ptr<AudioProcessor> processor
	) :
		reader{ std::move(reader) },
		processor{ std::move(processor) } {}

	void fillBuffer(float **channels, int) override {
		reader->read(channels, 0);
		processor->process(channels, 0);
	}
};

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

TEST(ProcessedAudioStreamTestCase, tbd) {
	const auto reader = std::make_shared<ReadsAOne>();
	const auto processor = std::make_shared<AudioTimesTwo>();
	ProcessedAudioStream stream{ reader, processor };
	float x{};
	float *channels[] = { &x };
	stream.fillBuffer(channels, 0);
	EXPECT_EQ(2, x);
}