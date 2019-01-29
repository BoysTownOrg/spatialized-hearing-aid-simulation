#include "AudioProcessingLoader.h"

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

AudioProcessingLoader::AudioProcessingLoader() :
	reader{ std::make_shared<NullReader>() },
	processor{ std::make_shared<NullProcessor>() },
	paddedZeros{ 0 } {}

void AudioProcessingLoader::reset() {
	paddedZeros = 0;
}

void AudioProcessingLoader::setReader(std::shared_ptr<AudioFrameReader> r) {
	reader = std::move(r);
}

void AudioProcessingLoader::setProcessor(std::shared_ptr<AudioFrameProcessor> p) {
	processor = std::move(p);
}

void AudioProcessingLoader::load(gsl::span<channel_type> audio) {
	const auto zerosToPad = audio.size() 
		? audio.begin()->size() - reader->remainingFrames() 
		: 0;
	reader->read(audio);
	if (zerosToPad > 0) {
		for (auto channel : audio)
			std::fill(
				channel.end() - gsl::narrow<channel_type::index_type>(zerosToPad), 
				channel.end(), 
				channel_type::element_type{ 0 }
			);
		paddedZeros += zerosToPad;
	}
	processor->process(audio);
}

bool AudioProcessingLoader::complete() {
	return reader->remainingFrames() == 0 && paddedZeros >= processor->groupDelay();
}
