#include "AudioDeviceController.h"

AudioDeviceController::AudioDeviceController(
	std::shared_ptr<AudioDevice> device, 
	std::shared_ptr<AudioStream> stream
) :
	device{ std::move(device) },
	stream{ std::move(stream) }
{
	this->device->setController(this);
}

void AudioDeviceController::startStreaming() {
	device->startStream();
}

void AudioDeviceController::stopStreaming() {
	device->stopStream();
}

void AudioDeviceController::fillStreamBuffer(void *channels, int frameCount) {
	const auto _channels = static_cast<float **>(channels);
	stream->fillBuffer(_channels[0], _channels[1], frameCount);
}
