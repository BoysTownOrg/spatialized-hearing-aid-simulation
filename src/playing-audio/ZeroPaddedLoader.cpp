#include "ZeroPaddedLoader.h"

class NullProcessor : public AudioFrameProcessor {
	void process(gsl::span<channel_type>) override {}
	channel_type::index_type groupDelay() override { return 0; }
};

class NullReader : public AudioFrameReader {
	void read(gsl::span<channel_type>) override {}
	bool complete() override { return true; }
	int sampleRate() override { return 0; }
	int channels() override { return 0; }
	long long frames() override { return 0; }
	void reset() override {}
	long long remainingFrames() override { return 0; }
};

ZeroPaddedLoader::ZeroPaddedLoader() noexcept :
	reader{ std::make_shared<NullReader>() },
	processor{ std::make_shared<NullProcessor>() },
	paddedZeros{ 0 } {}

void ZeroPaddedLoader::reset() {
	paddedZeros = 0;
}

void ZeroPaddedLoader::setReader(std::shared_ptr<AudioFrameReader> r) {
	reader = std::move(r);
}

void ZeroPaddedLoader::setProcessor(std::shared_ptr<AudioFrameProcessor> p) {
	processor = std::move(p);
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
