#pragma once
#include <audio-file-reading/AudioFileReader.h>
#include <vector>

class MockAudioFileReader : public AudioFileReader {
	std::vector<float> contents;
	int _channels;
public:
	explicit MockAudioFileReader(
		std::vector<float> contents,
		int _channels = 1
	) :
		contents{ std::move(contents) },
		_channels(_channels) {}

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