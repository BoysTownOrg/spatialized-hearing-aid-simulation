#include "AudioDevicePlayer.h"

AudioDevicePlayer::AudioDevicePlayer(AudioDevice *device) :
	device{ device }
{
	throwIfDeviceFailed<DeviceFailure>();
	device->setController(this);
}

template<typename exception>
void AudioDevicePlayer::throwIfDeviceFailed() {
	if (device->failed())
		throw exception{ device->errorMessage() };
}

void AudioDevicePlayer::setAudioLoader(AudioLoader *a) {
	loader = a;
}

void AudioDevicePlayer::prepareToPlay(Preparation p) {
	if (device->streaming())
		return;
	prepareToPlay_(std::move(p));
}

void AudioDevicePlayer::prepareToPlay_(Preparation p) {
	audio.resize(p.channels);
	reopenStream(std::move(p));
}

void AudioDevicePlayer::reopenStream(Preparation p) {
	device->closeStream();
	openStream(std::move(p));
	throwIfDeviceFailed<PreparationFailure>();
	device->setCallbackResultToContinue();
}

void AudioDevicePlayer::openStream(Preparation p) {
	AudioDevice::StreamParameters streaming;
	streaming.sampleRate = p.sampleRate;
	streaming.channels = p.channels;
	streaming.framesPerBuffer = p.framesPerBuffer;
	streaming.deviceIndex = findDeviceIndex(p.audioDevice);
	device->openStream(std::move(streaming));
}

int AudioDevicePlayer::findDeviceIndex(std::string deviceName) {
	int deviceIndex{};
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == deviceName)
			deviceIndex = i;
	return deviceIndex;
}

void AudioDevicePlayer::play() {
	device->startStream();
}

void AudioDevicePlayer::stop() {
	device->stopStream();
}

void AudioDevicePlayer::fillStreamBuffer(void * channels, int frames) {
	prepareAudioForLoading(channels, frames);
	loader->load(audio);
	signalDeviceIfDoneLoading();
}

void AudioDevicePlayer::prepareAudioForLoading(void * channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
}

void AudioDevicePlayer::signalDeviceIfDoneLoading() {
	if (loader->complete())
		device->setCallbackResultToComplete();
}

std::vector<std::string> AudioDevicePlayer::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}

bool AudioDevicePlayer::isPlaying() {
	return device->streaming();
}

