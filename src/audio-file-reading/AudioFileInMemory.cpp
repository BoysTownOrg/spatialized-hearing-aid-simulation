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

int AudioFileInMemory::samplesRemaining()
{
	return 0;
}

void AudioFileInMemory::readLeftChannel(float *x, int samples) {
	int i = 0;
	while (leftHead < left.size() && i < samples)
		x[i++] = left[leftHead++];
}

void AudioFileInMemory::readRightChannel(float *x, int samples) {
	int i = 0;
	while (rightHead < right.size() && i < samples)
		x[i++] = right[rightHead++];
}