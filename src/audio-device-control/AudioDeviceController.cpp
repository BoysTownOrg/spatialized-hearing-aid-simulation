#include "AudioDeviceController.h"

AudioDeviceController::AudioDeviceController(
	std::shared_ptr<AudioDevice> device, 
	std::shared_ptr<AudioFrameReader> stream
) :
	device{ std::move(device) },
	reader{ std::move(stream) }
{
	this->device->setController(this);
}

void AudioDeviceController::startStreaming() {
	device->startStream();
}

void AudioDeviceController::stopStreaming() {
	device->stopStream();
}

void AudioDeviceController::fillStreamBuffer(void *frames, int frameCount) {
	reader->read(static_cast<float **>(frames), frameCount);
}
