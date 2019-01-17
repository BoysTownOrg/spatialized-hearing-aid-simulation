#include "AudioProcessorImpl.h"

AudioProcessorImpl::AudioProcessorImpl(
	AudioFrameReaderFactory *readerFactory, 
	AudioFrameProcessorFactory *processorFactory
) :
	readerFactory{ readerFactory },
	processorFactory{ processorFactory } 
{
}

void AudioProcessorImpl::initialize(Initialization initialization) {
	processing.attack_ms = initialization.attack_ms;
	processing.release_ms = initialization.release_ms;
	processing.brirFilePath = initialization.brirFilePath;
	processing.leftDslPrescriptionFilePath = initialization.leftDslPrescriptionFilePath;
	processing.rightDslPrescriptionFilePath = initialization.rightDslPrescriptionFilePath;
	processing.chunkSize = initialization.chunkSize;
	processing.windowSize = initialization.windowSize;
	processing.max_dB_Spl = initialization.max_dB_Spl;
	processing.channelScalars.resize(2);
	try {
		processorFactory->make(processing);
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw InitializationFailure{ e.what() };
	}
	processing.channelScalars.clear();
}

class RmsComputer {
	std::vector<std::vector<float>> entireAudioFile;
public:
	explicit RmsComputer(AudioFrameReader &reader) :
		entireAudioFile{ reader.channels() }
	{
		std::vector<gsl::span<float>> pointers;
		for (auto &channel : entireAudioFile) {
			channel.resize(gsl::narrow<std::vector<float>::size_type>(reader.frames()));
			pointers.push_back({ channel });
		}
		reader.read(pointers);
	}

	double compute(int channel) {
		double squaredSum{};
		const auto channel_ = entireAudioFile.at(channel);
		for (const auto sample : channel_)
			squaredSum += sample * sample;
		return std::sqrt(squaredSum / channel_.size());
	}
};

void AudioProcessorImpl::prepare(Preparation p) {
	reader = makeReader(p.audioFilePath);
	const auto desiredRms = std::pow(10.0, (p.level_dB_Spl - processing.max_dB_Spl) / 20.0);
	RmsComputer rms{ *reader };
	for (int i = 0; i < reader->channels(); ++i)
		processing.channelScalars.push_back(desiredRms / rms.compute(i));
	processor = makeProcessor(processing);
	reader->reset();
}

std::shared_ptr<AudioFrameReader> AudioProcessorImpl::makeReader(std::string filePath) {
	try {
		return readerFactory->make(std::move(filePath));
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw PreparationFailure{ e.what() };
	}
}

std::shared_ptr<AudioFrameProcessor> AudioProcessorImpl::makeProcessor(
	AudioFrameProcessorFactory::Parameters p
) {
	try {
		return processorFactory->make(std::move(p));
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw PreparationFailure{ e.what() };
	}
}

void AudioProcessorImpl::process(gsl::span<gsl::span<float>> audio) {
	if (reader->complete()) {
		for (auto channel : audio)
			for (auto &x : channel) {
				++paddedZeroes;
				x = 0;
			}
	}
	reader->read(audio);
	processor->process(audio);
}

bool AudioProcessorImpl::complete() {
	return paddedZeroes >= processor->groupDelay();
}

int AudioProcessorImpl::channels() {
	return reader ? reader->channels() : 0;
}

int AudioProcessorImpl::sampleRate() {
	return reader ? reader->sampleRate() : 0;
}
