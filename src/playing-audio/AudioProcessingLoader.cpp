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

AudioProcessingLoader::AudioProcessingLoader(
	AudioFrameReaderFactory *readerFactory, 
	AudioFrameProcessorFactory *processorFactory
) :
	reader{std::make_shared<NullReader>()},
	processor{std::make_shared<NullProcessor>()},
	readerFactory{ readerFactory },
	processorFactory{ processorFactory } {}

void AudioProcessingLoader::prepare(Preparation p) {
	reader = makeReader(p.audioFilePath);
	AudioFrameProcessorFactory::Parameters processing;
//    processing.channelScalars = computeChannelScalars(p.level_dB_Spl);
	processing.channels = reader->channels();
	processing.sampleRate = reader->sampleRate();
	processor = makeProcessor(std::move(processing));
	reader->reset();
	paddedZeros = 0;
}

std::shared_ptr<AudioFrameReader> AudioProcessingLoader::makeReader(std::string filePath) {
	try {
		return readerFactory->make(std::move(filePath));
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw PreparationFailure{ e.what() };
	}
}

std::shared_ptr<AudioFrameProcessor> AudioProcessingLoader::makeProcessor(
	AudioFrameProcessorFactory::Parameters p
) {
	try {
		return processorFactory->make(std::move(p));
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw PreparationFailure{ e.what() };
	}
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

int AudioProcessingLoader::channels() {
	return reader->channels();
}

int AudioProcessingLoader::sampleRate() {
	return reader->sampleRate();
}
