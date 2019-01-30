#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(AudioDevice *device) :
	device{ device }
{
	throwIfDeviceFailed<DeviceFailure>();
	device->setController(this);
}

template<typename exception>
void AudioPlayer::throwIfDeviceFailed() {
	if (device->failed())
		throw exception{ device->errorMessage() };
}

void AudioPlayer::setAudioLoader(AudioLoader *a) {
	loader = a;
}

void AudioPlayer::prepareToPlay(Preparation p) {
	if (device->streaming())
		return;
	prepareToPlay_(std::move(p));
}

void AudioPlayer::prepareToPlay_(Preparation p) {
	audio.resize(p.channels);
	reopenStream(std::move(p));
}

void AudioPlayer::reopenStream(Preparation p) {
	device->closeStream();
	openStream(std::move(p));
	throwIfDeviceFailed<PreparationFailure>();
	device->setCallbackResultToContinue();
}

void AudioPlayer::openStream(Preparation p) {
	AudioDevice::StreamParameters streaming;
	streaming.sampleRate = p.sampleRate;
	streaming.channels = p.channels;
	streaming.framesPerBuffer = p.framesPerBuffer;
	streaming.deviceIndex = findDeviceIndex(p.audioDevice);
	device->openStream(std::move(streaming));
}

int AudioPlayer::findDeviceIndex(std::string deviceName) {
	int deviceIndex{};
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == deviceName)
			deviceIndex = i;
	return deviceIndex;
}

void AudioPlayer::play() {
	device->startStream();
}

void AudioPlayer::stop() {
	device->stopStream();
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	prepareAudioForLoading(channels, frames);
	loader->load(audio);
	signalDeviceIfDoneLoading();
}

void AudioPlayer::prepareAudioForLoading(void * channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio.at(i) = { static_cast<float **>(channels)[i], frames };
}

void AudioPlayer::signalDeviceIfDoneLoading() {
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

