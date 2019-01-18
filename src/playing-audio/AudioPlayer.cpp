#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(
	AudioDevice *device,
	AudioProcessor *processorFactory
) :
	device{ device },
	processor{ processorFactory }
{
	throwIfDeviceFailed<DeviceFailure>();
	device->setController(this);
}

template<typename exception>
void AudioPlayer::throwIfDeviceFailed() {
	if (device->failed())
		throw exception{ device->errorMessage() };
}

void AudioPlayer::initialize(Initialization init) {
	try {
		initializeProcessor(std::move(init));
	}
	catch (const AudioProcessor::InitializationFailure &e) {
		throw InitializationFailure{ e.what() };
	}
}

void AudioPlayer::initializeProcessor(Initialization request) {
	AudioProcessor::Initialization init;
	init.attack_ms = request.attack_ms;
	init.release_ms = request.release_ms;
	init.brirFilePath = request.brirFilePath;
	init.leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
	init.rightDslPrescriptionFilePath = request.rightDslPrescriptionFilePath;
	init.chunkSize = request.chunkSize;
	init.windowSize = request.windowSize;
	init.max_dB_Spl = request.max_dB_Spl;
	processor->initialize(init);
	framesPerBuffer_ = request.chunkSize;
}

void AudioPlayer::play(PlayRequest request) {
	if (device->streaming())
		return;
	play_(std::move(request));
}

void AudioPlayer::play_(PlayRequest request) {
	AudioProcessor::Preparation p;
	p.audioFilePath = request.audioFilePath;
	p.level_dB_Spl = request.level_dB_Spl;
	prepareProcessor(std::move(p));
	audio.resize(processor->channels());
	restartStream(std::move(request));
}

void AudioPlayer::prepareProcessor(AudioProcessor::Preparation p) {
	try {
		processor->prepare(std::move(p));
	}
	catch (const AudioProcessor::PreparationFailure &e) {
		throw RequestFailure{ e.what() };
	}
}

void AudioPlayer::restartStream(PlayRequest request) {
	device->closeStream();
	openStream(std::move(request));
	throwIfDeviceFailed<RequestFailure>();
	device->setCallbackResultToContinue();
	device->startStream();
}

void AudioPlayer::openStream(PlayRequest request) {
	AudioDevice::StreamParameters streaming;
	streaming.sampleRate = processor->sampleRate();
	streaming.channels = processor->channels();
	streaming.framesPerBuffer = framesPerBuffer_;
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			streaming.deviceIndex = i;
	device->openStream(std::move(streaming));
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	prepareAudio(channels, frames);
	processor->process(audio);
	completeIfDoneProcessing();
}

void AudioPlayer::prepareAudio(void * channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
}

void AudioPlayer::completeIfDoneProcessing() {
	if (processor->complete())
		device->setCallbackResultToComplete();
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

std::vector<int> AudioPlayer::preferredProcessingSizes() {
	return processor->preferredProcessingSizes();
}
