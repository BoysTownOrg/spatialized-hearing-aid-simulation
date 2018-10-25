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

TEST(AudioFileReadingTestCase, readEmptyFileReadsEmpty) {
	const auto reader = std::make_shared<MockAudioFileReader>(std::vector<float>{});
	AudioFileInMemory audioFile{ reader };
	assertEqual({}, audioFile.readLeftChannel(0));
	assertEqual({}, audioFile.readLeftChannel(1));
	assertEqual({}, audioFile.readLeftChannel(2));
}

TEST(AudioFileReadingTestCase, readChannelsSampleBySample) {
	const auto reader = 
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4, 5, 6 });
	reader->setChannels(2);
	AudioFileInMemory audioFile{ reader };
	assertEqual({ 3 }, audioFile.readLeftChannel(1));
	assertEqual({ 5 }, audioFile.readLeftChannel(1));
	assertEqual({ 4 }, audioFile.readRightChannel(1));
	assertEqual({ 6 }, audioFile.readRightChannel(1));
}

TEST(AudioFileReadingTestCase, readChannelExhaustedReturnsEmpty) {
	const auto reader =
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4, 5, 6 });
	reader->setChannels(2);
	AudioFileInMemory audioFile{ reader };
	assertEqual({ 3 }, audioFile.readLeftChannel(1));
	assertEqual({ 5 }, audioFile.readLeftChannel(1));
	EXPECT_TRUE(audioFile.readLeftChannel(1).empty());
}