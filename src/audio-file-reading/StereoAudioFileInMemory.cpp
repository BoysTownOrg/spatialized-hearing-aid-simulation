#include "StereoAudioFileInMemory.h"
#include <gsl/gsl_util>

StereoAudioFileInMemory::StereoAudioFileInMemory(AudioFileReader &reader) :
	buffer(gsl::narrow<std::size_t>(reader.frames() * reader.channels())),
	channels(reader.channels())
{
	if (!(channels == 1 || channels == 2))
		throw InvalidChannelCount{};
	if (buffer.size() == 0)
		return;
	reader.readFrames(&buffer[0], reader.frames());
}

int StereoAudioFileInMemory::framesRemaining() const
{
	return buffer.size() - head;
}

void StereoAudioFileInMemory::read(float *left, float *right, int samples) {
	int i = 0;
	while (head < buffer.size() && i < samples) {
		left[i] = buffer[head];
		if (channels == 2)
			++head;
		right[i++] = buffer[head++];
	}
}