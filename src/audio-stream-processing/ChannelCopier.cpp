#include "ChannelCopier.h"

ChannelCopier::ChannelCopier(std::shared_ptr<AudioFrameReader> reader) :
	reader{ std::move(reader) } {}

void ChannelCopier::read(gsl::span<channel_type> audio) {
	if (mono())
		readAndCopyFirstChannel(audio);
	else
		readAllChannels(audio);
}

void ChannelCopier::readAndCopyFirstChannel(gsl::span<channel_type> audio) {
	reader->read(audio.first(1));
	for (const auto channel : audio.last(audio.size() - 1))
		for (int i = 0; i < channel.size(); ++i)
			channel[i] = (*audio.begin())[i];
}

void ChannelCopier::readAllChannels(gsl::span<channel_type> audio) {
	reader->read(audio);
}

bool ChannelCopier::complete() {
	return reader->complete();
}

int ChannelCopier::sampleRate() {
	return reader->sampleRate();
}

int ChannelCopier::channels() {
	return mono() ? 2 : reader->channels();
}

bool ChannelCopier::mono() {
	return reader->channels() == 1;
}

long long ChannelCopier::frames() {
	return reader->frames();
}

void ChannelCopier::reset() {
	reader->reset();
}

long long ChannelCopier::remainingFrames() {
    return reader->remainingFrames();
}

ChannelCopierFactory::ChannelCopierFactory(
	std::shared_ptr<AudioFrameReaderFactory> factory
) :
	factory{ std::move(factory) }
{
}

std::shared_ptr<AudioFrameReader> ChannelCopierFactory::make(std::string filePath) {
	return std::make_shared<ChannelCopier>(factory->make(std::move(filePath)));
}
