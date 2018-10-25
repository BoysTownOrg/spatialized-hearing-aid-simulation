#include <common-includes/Interface.h>

class AudioFileReader {
public:
	INTERFACE_OPERATIONS(AudioFileReader);
	virtual long long frames() = 0;
	virtual int channels() = 0;
	virtual void readFrames(float *, long long) = 0;
};

#include <memory>
#include <vector>

class AudioFileInMemory {
	std::vector<float> left;
	std::vector<float> right;
	std::size_t leftHead = 0;
	std::size_t rightHead = 0;
public:
	explicit AudioFileInMemory(
		std::shared_ptr<AudioFileReader> reader
	)
	{
		std::vector<float> buffer(
			static_cast<std::size_t>(reader->frames() * reader->channels()));
		reader->readFrames(&buffer[0], reader->frames());
		for (std::size_t i = 0; i < buffer.size() - 1; i += 2) {
			left.push_back(buffer[i]);
			right.push_back(buffer[i + 1]);
		}
	}
	std::vector<float> readLeftChannel(int samples) {
		const auto next = std::vector<float>(
			left.begin() + leftHead, 
			left.begin() + leftHead + samples);
		leftHead += samples;
		return next;
	}
	std::vector<float> readRightChannel(int samples) {
		const auto next = std::vector<float>(
			right.begin() + rightHead,
			right.begin() + rightHead + samples);
		rightHead += samples;
		return next;
	}
};

#include "assert-utility.h"
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
		return contents.size() / _channels;
	}
	int channels() override
	{
		return _channels;
	}
	void readFrames(float *x, long long n) override
	{
		std::memcpy(x, &contents[0], static_cast<std::size_t>(n) * sizeof(float) * _channels);
	}
};

class AudioFileReadingTestCase : public ::testing::TestCase {};

TEST(AudioFileReadingTestCase, tbd) {
	const auto reader = 
		std::make_shared<MockAudioFileReader>(std::vector<float>{ 3, 4, 5, 6 });
	reader->setChannels(2);
	AudioFileInMemory audioFile{ reader };
	assertEqual({ 3 }, audioFile.readLeftChannel(1));
	assertEqual({ 5 }, audioFile.readLeftChannel(1));
	assertEqual({ 4 }, audioFile.readRightChannel(1));
	assertEqual({ 6 }, audioFile.readRightChannel(1));
}