#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(
	AudioDevice *device,
	AudioLoader *loader
) :
	device{ device },
	loader{ loader }
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
		initializeLoader(std::move(init));
	}
	catch (const AudioLoader::InitializationFailure &e) {
		throw InitializationFailure{ e.what() };
	}
}

void AudioPlayer::initializeLoader(Initialization request) {
	AudioLoader::Initialization init;
	init.attack_ms = request.attack_ms;
	init.release_ms = request.release_ms;
	init.brirFilePath = request.brirFilePath;
	init.leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
	init.rightDslPrescriptionFilePath = request.rightDslPrescriptionFilePath;
	init.chunkSize = request.chunkSize;
	init.windowSize = request.windowSize;
	init.max_dB_Spl = request.max_dB_Spl;
	loader->initialize(std::move(init));
}

void AudioPlayer::play(PlayRequest request) {
	if (device->streaming())
		return;
	play_(std::move(request));
}

void AudioPlayer::play_(PlayRequest request) {
	AudioLoader::Preparation p;
	p.audioFilePath = request.audioFilePath;
	p.level_dB_Spl = request.level_dB_Spl;
	prepareLoader(std::move(p));
	audio.resize(loader->channels());
	restartStream(request.audioDevice);
}

void AudioPlayer::prepareLoader(AudioLoader::Preparation p) {
	try {
		loader->prepare(std::move(p));
	}
	catch (const AudioLoader::PreparationFailure &e) {
		throw RequestFailure{ e.what() };
	}
}

void AudioPlayer::restartStream(std::string deviceName) {
	device->closeStream();
	openStream(std::move(deviceName));
	throwIfDeviceFailed<RequestFailure>();
	device->setCallbackResultToContinue();
	device->startStream();
}

void AudioPlayer::openStream(std::string deviceName) {
	AudioDevice::StreamParameters streaming;
	streaming.sampleRate = loader->sampleRate();
	streaming.channels = loader->channels();
	streaming.framesPerBuffer = loader->chunkSize();
	streaming.deviceIndex = findDeviceIndex(std::move(deviceName));
	device->openStream(std::move(streaming));
}

int AudioPlayer::findDeviceIndex(std::string deviceName) {
	int deviceIndex{};
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == deviceName)
			deviceIndex = i;
	return deviceIndex;
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	prepareAudio(channels, frames);
	loader->load(audio);
	completeIfDoneProcessing();
}

void AudioPlayer::prepareAudio(void * channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
}

void AudioPlayer::completeIfDoneProcessing() {
	if (loader->complete())
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
	return loader->preferredProcessingSizes();
}
