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

int AudioFileInMemory::framesRemaining() const
{
	return buffer.size() - head;
}

void AudioFileInMemory::read(float *left, float *right, int frames) {
	int i = 0;
	while (head < buffer.size() && i < frames) {
		left[i] = buffer[head];
		if (channels == 2)
			++head;
		right[i++] = buffer[head++];
	}
}