#include "ChannelCopier.h"

ChannelCopier::ChannelCopier(std::shared_ptr<AudioFrameReader> reader) :
	reader{ std::move(reader) } {}

void ChannelCopier::read(gsl::span<gsl::span<float>> audio) {
	if (reader->channels() == 1) {
		reader->read(audio.first(1));
		for (const auto channel : audio.last(audio.size() - 1))
			for (int i = 0; i < audio.begin()->size(); ++i)
				channel[i] = (*audio.begin())[i];
	}
	else
		reader->read(audio);
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
