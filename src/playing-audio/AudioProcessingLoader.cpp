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
	long long framesRemaining() override { return 0; }
};

AudioProcessingLoader::AudioProcessingLoader(
	AudioFrameReaderFactory *readerFactory, 
	AudioFrameProcessorFactory *processorFactory
) :
	reader{std::make_shared<NullReader>()},
	processor{std::make_shared<NullProcessor>()},
	readerFactory{ readerFactory },
	processorFactory{ processorFactory } {}

void AudioProcessingLoader::initialize(Initialization init) {
	try {
		processorFactory->assertCanBeMade(init.global);
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw InitializationFailure{ e.what() };
	}
	processorFactory->storeParameters(init.global);
}

void AudioProcessingLoader::storeProcessingParameters(AudioLoader::Initialization init) {
	processing.global = init.global;
	processing.channelScalars.resize(2);
}

void AudioProcessingLoader::assertProcessorCanBeMade() {
	try {
		processorFactory->make(processing);
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw InitializationFailure{ e.what() };
	}
}

void AudioProcessingLoader::prepare(Preparation p) {
	reader = makeReader(p.audioFilePath);
    processing.channelScalars = computeChannelScalars(p.level_dB_Spl);
	processing.channels = reader->channels();
	processing.sampleRate = reader->sampleRate();
	processor = makeProcessor(processing);
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

class RmsComputer {
    std::vector<std::vector<float>> entireAudioFile;
public:
	explicit RmsComputer(AudioFrameReader &reader) :
		entireAudioFile(
			reader.channels(), 
			std::vector<float>(gsl::narrow<std::vector<float>::size_type>(reader.frames()))
		)
	{
		std::vector<gsl::span<float>> pointers;
		for (auto &channel : entireAudioFile)
			pointers.push_back({ channel });
		reader.read(pointers);
	}

    double compute(int channel) {
        double squaredSum{};
        const auto channel_ = entireAudioFile.at(channel);
        for (const double sample : channel_)
            squaredSum += sample * sample;
        return std::sqrt(squaredSum / channel_.size());
    }
};

std::vector<double> AudioProcessingLoader::computeChannelScalars(double level_dB_Spl) {
    RmsComputer rms{ *reader };
    const auto desiredRms = std::pow(10.0, (level_dB_Spl - processorFactory->fullScale_dB_Spl()) / 20.0);
    std::vector<double> scalars{};
    for (int i = 0; i < reader->channels(); ++i)
        scalars.push_back(desiredRms / rms.compute(i));
    return scalars;
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

int AudioProcessingLoader::bufferSize() {
	return processorFactory->preferredBufferSize();
}

std::vector<int> AudioProcessingLoader::preferredProcessingSizes() {
	return processorFactory->preferredProcessingSizes();
}

void AudioProcessingLoader::load(gsl::span<gsl::span<float>> audio) {
	const auto zerosToPad = audio.begin()->size() - reader->framesRemaining();
	reader->read(audio);
	if (zerosToPad > 0) {
		for (auto channel : audio)
			for (int i = 0; i < zerosToPad; ++i)
				*(channel.end() - i - 1) = 0;
		paddedZeros += zerosToPad;
	}
	processor->process(audio);
}

bool AudioProcessingLoader::complete() {
	return paddedZeros >= processor->groupDelay();
}

int AudioProcessingLoader::channels() {
	return reader->channels();
}

int AudioProcessingLoader::sampleRate() {
	return reader->sampleRate();
}
