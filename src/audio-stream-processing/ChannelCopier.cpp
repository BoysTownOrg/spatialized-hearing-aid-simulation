#include "ChannelCopier.h"

ChannelCopier::ChannelCopier(std::shared_ptr<AudioFrameReader> reader) :
	reader{ std::move(reader) } {}

void ChannelCopier::read(gsl::span<float *> audio, int frames) {
	reader->read(audio, frames);
	if (reader->channels() == 1)
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
	return reader->channels() == 1 ? 2 : reader->channels();
}

long long ChannelCopier::frames() const {
	return reader->frames();
}

void ChannelCopier::reset() {
	reader->reset();
}

ChannelCopierFactory::ChannelCopierFactory(
	std::shared_ptr<AudioFrameReaderFactory> factory
) :
	factory{ std::move(factory) }
{
}

std::shared_ptr<AudioFrameReader> ChannelCopierFactory::make(std::string filePath) {
	return std::make_shared<ChannelCopier>(factory->make(filePath));
}
