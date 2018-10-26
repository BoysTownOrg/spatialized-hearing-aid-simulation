#include "AudioFileInMemory.h"

AudioFileInMemory::AudioFileInMemory(std::shared_ptr<AudioFileReader> reader) :
	buffer(static_cast<std::size_t>(reader->frames() * reader->channels())),
	channels(reader->channels())
{
	if (!(channels == 1 || channels == 2))
		throw InvalidChannelCount{};
	if (buffer.size() == 0)
		return;
	reader->readFrames(&buffer[0], reader->frames());
}

int AudioFileInMemory::framesRemaining()
{
	return buffer.size() - head;
}

void AudioFileInMemory::read(float *left, float *right, int frames) {
	int i = 0;
	while (head < buffer.size() && i < frames) {
		if (channels == 2) {
			left[i] = buffer[head++];
			right[i++] = buffer[head++];
		}
		else {
			left[i] = buffer[head];
			right[i++] = buffer[head++];
		}
	}
}