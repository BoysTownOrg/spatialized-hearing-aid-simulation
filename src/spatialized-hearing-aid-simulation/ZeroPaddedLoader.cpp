#include "ZeroPaddedLoader.h"

ZeroPaddedLoader::ZeroPaddedLoader(
	std::shared_ptr<AudioFrameReader> reader,
	std::shared_ptr<AudioFrameProcessor> processor
) noexcept :
	processor{ std::move(processor) },
    reader{ std::move(reader) },
	paddedZeros{ 0 } {}

void ZeroPaddedLoader::reset() {
	paddedZeros = 0;
}

void ZeroPaddedLoader::load(gsl::span<channel_type> audio) {
	const auto zerosToPad = audio.size() 
		? audio.begin()->size() - reader->remainingFrames() 
		: 0;
	reader->read(audio);
	if (zerosToPad > 0)
		padZeros(audio, zerosToPad);
	processor->process(audio);
}

void ZeroPaddedLoader::padZeros(gsl::span<channel_type> audio, long long zerosToPad) {
	for (auto channel : audio)
		std::fill(
			channel.end() - gsl::narrow<channel_type::index_type>(zerosToPad),
			channel.end(),
			channel_type::element_type{ 0 }
		);
	paddedZeros += zerosToPad;
}

bool ZeroPaddedLoader::complete() {
	return reader->remainingFrames() == 0 && paddedZeros >= processor->groupDelay();
}

std::shared_ptr<AudioLoader> ZeroPaddedLoaderFactory::make(
	std::shared_ptr<AudioFrameReader> r, 
	std::shared_ptr<AudioFrameProcessor> p
) {
	return std::make_shared<ZeroPaddedLoader>(std::move(r), std::move(p));
}
