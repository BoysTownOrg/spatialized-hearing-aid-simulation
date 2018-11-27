#include "AudioFileInMemory.h"
#include <gsl/gsl>

AudioFileInMemory::AudioFileInMemory(AudioFileReader &reader) :
	buffer(gsl::narrow<size_type>(reader.frames() * reader.channels())),
	channelCount(reader.channels())
{
	if (buffer.size() == 0)
		return;
	reader.readFrames(&buffer[0], reader.frames());
}

void AudioFileInMemory::read(float ** channels, int frameCount) {
	for (int i = 0; i < frameCount; ++i)
		for (int j = 0; j < channelCount; ++j) {
			if (head == buffer.size())
				return;
			channels[j][i] = buffer[head++];
		}
}
