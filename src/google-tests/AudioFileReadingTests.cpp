#include "assert-utility.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

class MockAudioFileReader : public AudioFileReader {
	std::vector<float> contents;
	int _channels{};
public:
	explicit MockAudioFileReader(std::vector<float> contents) :
		contents{ std::move(contents) } {}

	void setChannels(int c) {
		_channels = c;
	}

	long long frames() override
	{
		return _channels == 0 ? 0 : contents.size() / _channels;
	}

	int channels() override
	{
		return _channels;
	}

	void readFrames(float *x, long long n) override
	{
		if (contents.size() == 0)
			return;
		std::memcpy(
			x,
			&contents[0], 
			static_cast<std::size_t>(n) * sizeof(float) * _channels);
	}
};

class AudioFileReadingTestCase : public ::testing::TestCase {};

TEST(AudioFileReadingTestCase, readChannelsSampleBySample) {
	const auto reader = 
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4, 5, 6 });
	reader->setChannels(2);
	AudioFileInMemory audioFile{ reader };
	float x{};
	audioFile.readLeftChannel(&x, 1);
	EXPECT_EQ(3, x);
	audioFile.readLeftChannel(&x, 1);
	EXPECT_EQ(5, x);
	audioFile.readRightChannel(&x, 1);
	EXPECT_EQ(4, x);
	audioFile.readRightChannel(&x, 1);
	EXPECT_EQ(6, x);
}

TEST(AudioFileReadingTestCase, readChannelReadsNothingWhenExhausted) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4 });
	reader->setChannels(2);
	AudioFileInMemory audioFile{ reader };
	float x{};
	audioFile.readLeftChannel(&x, 1);
	audioFile.readLeftChannel(&x, 1);
	EXPECT_EQ(3, x);
	audioFile.readRightChannel(&x, 1);
	audioFile.readRightChannel(&x, 1);
	EXPECT_EQ(4, x);
}

TEST(AudioFileReadingTestCase, readChannelReadsLessThanRequested) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4 });
	reader->setChannels(2);
	AudioFileInMemory audioFile{ reader };
	float x[2] = {};
	audioFile.readLeftChannel(x, 2);
	EXPECT_EQ(3, x[0]);
	EXPECT_EQ(0, x[1]);
	audioFile.readRightChannel(x, 2);
	EXPECT_EQ(4, x[0]);
	EXPECT_EQ(0, x[1]);
}