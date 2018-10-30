#include "AudioDeviceController.h"

AudioDeviceController::AudioDeviceController(
	std::shared_ptr<AudioDevice> device, 
	std::shared_ptr<AudioFrameReader> reader
) :
	device{ std::move(device) },
	reader{ std::move(reader) }
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
	reader->read(static_cast<float **>(channels), frameCount);
}
