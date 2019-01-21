#include "AudioProcessingLoader.h"

AudioProcessingLoader::AudioProcessingLoader(
	AudioFrameReaderFactory *readerFactory, 
	AudioFrameProcessorFactory *processorFactory
) :
	readerFactory{ readerFactory },
	processorFactory{ processorFactory } {}

void AudioProcessingLoader::initialize(Initialization init) {
	storeProcessingParameters(std::move(init));
	assertProcessorCanBeMade();
}

void AudioProcessingLoader::storeProcessingParameters(AudioLoader::Initialization init) {
	processing.attack_ms = init.attack_ms;
	processing.release_ms = init.release_ms;
	processing.brirFilePath = init.brirFilePath;
	processing.leftDslPrescriptionFilePath = init.leftDslPrescriptionFilePath;
	processing.rightDslPrescriptionFilePath = init.rightDslPrescriptionFilePath;
	processing.chunkSize = init.chunkSize;
	processing.windowSize = init.windowSize;
	processing.max_dB_Spl = init.max_dB_Spl;
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
	paddedZeroes = 0;
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
    const auto desiredRms = std::pow(10.0, (level_dB_Spl - processing.max_dB_Spl) / 20.0);
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

int AudioProcessingLoader::chunkSize() {
	return processing.chunkSize;
}

std::vector<int> AudioProcessingLoader::preferredProcessingSizes() {
	return processorFactory->preferredProcessingSizes();
}

void AudioProcessingLoader::load(gsl::span<gsl::span<float>> audio) {
	if (reader->framesRemaining() < audio.begin()->size()) {
        for (auto channel : audio)
            for (int i = 0; i < audio.begin()->size() - reader->framesRemaining(); ++i)
                *(channel.end() - i - 1) = 0;
		paddedZeroes += audio.begin()->size();
	}
	else
		reader->read(audio);
	processor->process(audio);
}

bool AudioProcessingLoader::complete() {
	return processor ? paddedZeroes >= processor->groupDelay() : true;
}

int AudioProcessingLoader::channels() {
	return reader ? reader->channels() : 0;
}

int AudioProcessingLoader::sampleRate() {
	return reader ? reader->sampleRate() : 0;
}
