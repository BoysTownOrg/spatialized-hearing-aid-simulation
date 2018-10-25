#include "AudioFileInMemory.h"

AudioFileInMemory::AudioFileInMemory(std::shared_ptr<AudioFileReader> reader) {
	std::vector<float> buffer(
		static_cast<std::size_t>(reader->frames() * reader->channels()));
	if (buffer.size() == 0)
		return;
	reader->readFrames(&buffer[0], reader->frames());
	for (std::size_t i = 0; i < buffer.size() - 1; i += 2) {
		left.push_back(buffer[i]);
		right.push_back(buffer[i + 1]);
	}
}

std::vector<float> AudioFileInMemory::readLeftChannel(int samples) {
	if (samples >= left.size()) {
		const auto next = left;
		left.clear();
		return next;
	}
	const auto next = std::vector<float>(
		left.begin(),
		left.begin() + samples);
	left.erase(left.begin(), left.begin() + samples);
	return next;
}

std::vector<float> AudioFileInMemory::readRightChannel(int samples) {
	if (right.size() == 0)
		return {};
	const auto next = std::vector<float>(
		right.begin(),
		right.begin() + samples);
	right.erase(right.begin(), right.begin() + samples);
	return next;
}