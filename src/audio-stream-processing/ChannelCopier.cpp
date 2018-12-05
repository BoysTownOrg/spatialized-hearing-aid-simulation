#include "ChannelCopier.h"

ChannelCopier::ChannelCopier(std::shared_ptr<AudioFrameReader> reader) :
	reader{ std::move(reader) } {}

inline void ChannelCopier::read(float ** channels, int frameCount)
{
	reader->read(channels, frameCount);
	for (int i = 0; i < frameCount; ++i)
		channels[1][i] = channels[0][i];
}

bool ChannelCopier::complete() const {
	return reader->complete();
}

int ChannelCopier::sampleRate() const
{
	return 0;
}
