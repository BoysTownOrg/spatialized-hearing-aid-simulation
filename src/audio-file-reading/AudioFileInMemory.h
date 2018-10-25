#pragma once

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

