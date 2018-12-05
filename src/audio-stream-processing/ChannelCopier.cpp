#include "ChannelCopier.h"

ChannelCopier::ChannelCopier(std::shared_ptr<AudioFrameReader> reader) :
	reader{ std::move(reader) } {}

void ChannelCopier::read(float **audio, int frames) {
	reader->read(audio, frames);
	for (int i = 0; i < frames; ++i)
		audio[1][i] = audio[0][i];
}

bool ChannelCopier::complete() const {
	return reader->complete();
}

int ChannelCopier::sampleRate() const {
	return reader->sampleRate();
}

int ChannelCopier::channels() const {
	return 2;
}

ChannelCopierFactory::ChannelCopierFactory(
	std::shared_ptr<AudioFrameReaderFactory> factory
) :
	factory{ std::move(factory) }
{
}

std::shared_ptr<AudioFrameReader> ChannelCopierFactory::make(std::string filePath) {
	auto frameReader = factory->make(filePath);
	if (frameReader->channels() == 1)
		frameReader = std::make_shared<ChannelCopier>(frameReader);
	return frameReader;
}
