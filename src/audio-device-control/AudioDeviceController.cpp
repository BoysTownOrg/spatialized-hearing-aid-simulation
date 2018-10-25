#include "AudioDeviceController.h"

AudioDeviceController::AudioDeviceController(
	std::shared_ptr<AudioDevice> device, 
	std::shared_ptr<AudioStream> stream
) :
	device{ std::move(device) }
{
	this->device->setController(this);
}

void AudioDeviceController::startStreaming() {
	device->startStream();
}

void AudioDeviceController::stopStreaming() {
	device->stopStream();
}
