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

void AudioPlayer::initialize(Initialization request) {
	processing.attack_ms = request.attack_ms;
	processing.release_ms = request.release_ms;
	processing.brirFilePath = request.brirFilePath;
	processing.leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
	processing.rightDslPrescriptionFilePath = request.rightDslPrescriptionFilePath;
	processing.chunkSize = request.chunkSize;
	processing.windowSize = request.windowSize;
	processing.max_dB_Spl = request.max_dB_Spl;
	try {
		processorFactory->make(processing);
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw InitializationFailure{ e.what() };
	}
}

void AudioPlayer::play(PlayRequest request) {
	if (device->streaming())
		return;
	
	frameReader = makeReader(request.audioFilePath);
	audio.resize(frameReader->channels());

	processing.channels = frameReader->channels();
	processing.sampleRate = frameReader->sampleRate();
	processing.level_dB_Spl = request.level_dB_Spl;
	frameProcessor = makeProcessor(processing);

	AudioDevice::StreamParameters streaming;
	streaming.sampleRate = frameReader->sampleRate();
	streaming.channels = frameReader->channels();
	streaming.framesPerBuffer = processing.chunkSize;
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			streaming.deviceIndex = i;

	device->closeStream();
	device->openStream(streaming);
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
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
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
	if (frameProcessor->complete())
		device->setCallbackResultToComplete();
	frameProcessor->process(audio);
}

std::vector<std::string> AudioPlayer::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}

bool AudioPlayer::isPlaying() {
	return device->streaming();
}
