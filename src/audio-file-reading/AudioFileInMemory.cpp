#include "AudioFileInMemory.h"

AudioFileInMemory::AudioFileInMemory(std::shared_ptr<AudioFileReader> reader) {
	std::vector<float> buffer(
		static_cast<std::size_t>(reader->frames() * reader->channels()));
	reader->readFrames(&buffer[0], reader->frames());
	for (std::size_t i = 0; i < buffer.size() - 1; i += 2) {
		left.push_back(buffer[i]);
		right.push_back(buffer[i + 1]);
	}
}

std::vector<float> AudioFileInMemory::readLeftChannel(int samples) {
	const auto next = std::vector<float>(
		left.begin() + leftHead,
		left.begin() + leftHead + samples);
	leftHead += samples;
	return next;
}

std::vector<float> AudioFileInMemory::readRightChannel(int samples) {
	const auto next = std::vector<float>(
		right.begin() + rightHead,
		right.begin() + rightHead + samples);
	rightHead += samples;
	return next;
}