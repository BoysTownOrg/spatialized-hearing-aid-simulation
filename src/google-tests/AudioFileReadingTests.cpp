#include "assert-utility.h"
#include <audio-file-reading/AudioFileInMemory.h>
#include <gtest/gtest.h>

class MockAudioFileReader : public AudioFileReader {
	std::vector<float> contents;
	int _channels = 1;
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

TEST(AudioFileReadingTestCase, emptyFileHasZeroSamplesRemaining) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{});
	AudioFileInMemory file{ reader };
	EXPECT_EQ(0, file.samplesRemaining());
}

TEST(AudioFileReadingTestCase, missingSamplesThrows) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 4, 5, 6 });
	reader->setChannels(2);
	EXPECT_THROW(AudioFileInMemory file{ reader }, AudioFileInMemory::InvalidFrameCount);
}

TEST(AudioFileReadingTestCase, constructorThrowsIfNotMonoOrStereo) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{});
	reader->setChannels(0);
	EXPECT_THROW(AudioFileInMemory file{ reader }, AudioFileInMemory::InvalidChannelCount);
	reader->setChannels(3);
	EXPECT_THROW(AudioFileInMemory file{ reader }, AudioFileInMemory::InvalidChannelCount);
}

TEST(AudioFileReadingTestCase, readChannelsSampleBySample) {
	const auto reader = 
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4, 5, 6 });
	reader->setChannels(2);
	AudioFileInMemory file{ reader };
	float left{};
	float right{};
	file.read(&left, &right, 1);
	EXPECT_EQ(3, left);
	EXPECT_EQ(4, right);
	file.read(&left, &right, 1);
	EXPECT_EQ(5, left);
	EXPECT_EQ(6, right);
}

TEST(AudioFileReadingTestCase, readChannelReadsNothingWhenExhausted) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4 });
	reader->setChannels(2);
	AudioFileInMemory file{ reader };
	float left{};
	float right{};
	file.read(&left, &right, 1);
	file.read(&left, &right, 1);
	EXPECT_EQ(3, left);
	EXPECT_EQ(4, right);
}
