#include "AudioFileInMemory.h"
#include <gsl/gsl>

AudioFileInMemory::AudioFileInMemory(AudioFileReader &reader) :
	buffer(gsl::narrow<size_type>(reader.frames() * reader.channels())),
	_channels(reader.channels()),
	_sampleRate(reader.sampleRate())
{
	if (reader.failed())
		throw FileError{ reader.errorMessage() };
	if (buffer.size() == 0)
		return;
	reader.readFrames(&buffer[0], reader.frames());
}

void AudioFileInMemory::read(float **audio, int frames) {
	for (int i = 0; i < frames; ++i)
		for (int j = 0; j < _channels; ++j) {
			if (head == buffer.size())
				return;
			audio[j][i] = buffer[head++];
		}
}

bool AudioFileInMemory::complete() const {
	return head == buffer.size();
}

int AudioFileInMemory::sampleRate() const {
	return _sampleRate;
}

int AudioFileInMemory::channels() const {
	return _channels;
}
