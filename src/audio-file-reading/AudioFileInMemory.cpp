#include "AudioFileInMemory.h"

AudioFileInMemory::AudioFileInMemory(std::shared_ptr<AudioFileReader> reader) :
	buffer(static_cast<std::size_t>(reader->frames() * reader->channels())) 
{
	if (!(reader->channels() == 1 || reader->channels() == 2))
		throw InvalidChannelCount{};
	if (buffer.size() == 0)
		return;
	reader->readFrames(&buffer[0], reader->frames());
}

int AudioFileInMemory::framesRemaining()
{
	return 0;
}

void AudioFileInMemory::read(float *left, float *right, int samples) {
	int i = 0;
	while (head < buffer.size() && i < samples) {
		left[i] = buffer[head++];
		right[i++] = buffer[head++];
	}
}