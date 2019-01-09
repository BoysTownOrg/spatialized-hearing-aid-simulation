#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(
	AudioDevice *device, 
	AudioFrameReaderFactory *readerFactory, 
	AudioFrameProcessorFactory *processorFactory
) :
	device{ device },
	readerFactory{ readerFactory },
	processorFactory{ processorFactory }
{
	if (device->failed())
		throw DeviceFailure{ device->errorMessage() };
	device->setController(this);
}

static std::vector<double> computeStimulusRms(AudioFrameReader &reader) {
	std::vector<std::vector<float>> entireAudioFile(reader.channels());
	std::vector<gsl::span<float>> pointers;
	for (auto &channel : entireAudioFile) {
		channel.resize(gsl::narrow<std::vector<float>::size_type>(reader.frames()));
		pointers.push_back({ channel });
	}
	reader.read(pointers);
	std::vector<double> stimulusRms;
	for (const auto &channel : entireAudioFile) {
		float squaredSum{};
		for (const auto sample : channel)
			squaredSum += sample * sample;
		stimulusRms.push_back(std::sqrt(squaredSum / channel.size()));
	}
	return stimulusRms;
}

void AudioPlayer::play(PlayRequest request) {
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
	
	if (device->streaming())
		return;
	
	frameReader = makeReader(request.audioFilePath);
	audio.resize(frameReader->channels());

	AudioFrameProcessorFactory::Parameters forProcessor;
	forProcessor.attack_ms = request.attack_ms;
	forProcessor.release_ms = request.release_ms;
	forProcessor.channels = frameReader->channels();
	forProcessor.brirFilePath = request.brirFilePath;
	forProcessor.leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
	forProcessor.rightDslPrescriptionFilePath = request.rightDslPrescriptionFilePath;
	forProcessor.level_dB_Spl = request.level_dB_Spl;
	forProcessor.sampleRate = frameReader->sampleRate();
	forProcessor.chunkSize = request.chunkSize;
	forProcessor.windowSize = request.windowSize;
	forProcessor.stimulusRms = computeStimulusRms(*frameReader);
	frameProcessor = makeProcessor(forProcessor);

	frameReader->reset();

	AudioDevice::StreamParameters forStreaming;
	forStreaming.sampleRate = frameReader->sampleRate();
	forStreaming.channels = frameReader->channels();
	forStreaming.framesPerBuffer = request.chunkSize;
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			forStreaming.deviceIndex = i;

	device->closeStream();
	device->openStream(forStreaming);
	device->setCallbackResultToContinue();
	device->startStream();
}

std::shared_ptr<AudioFrameReader> AudioPlayer::makeReader(std::string filePath) {
	try {
		return readerFactory->make(std::move(filePath));
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

std::shared_ptr<AudioFrameProcessor> AudioPlayer::makeProcessor(
	AudioFrameProcessorFactory::Parameters p
) {
	try {
		return processorFactory->make(std::move(p));
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	if (frameReader->complete())
		device->setCallbackResultToComplete();
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
	frameReader->read(audio);
	frameProcessor->process(audio);
}

std::vector<std::string> AudioPlayer::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}
