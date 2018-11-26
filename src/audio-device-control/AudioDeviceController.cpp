#include "AudioDeviceController.h"

AudioDeviceController::AudioDeviceController(
	std::shared_ptr<AudioDevice> device, 
	std::shared_ptr<AudioFrameReader> reader
) :
	device{ std::move(device) },
	reader{ std::move(reader) }
{
	if (this->device->failed())
		throw DeviceConnectionFailure{ this->device->errorMessage() };
	this->device->setController(this);
}

void AudioDeviceController::startStreaming() {
	device->startStream();
	if (this->device->failed())
		throw StreamingError{ this->device->errorMessage() };
}

void AudioDeviceController::stopStreaming() {
	device->stopStream();
	if (this->device->failed())
		throw StreamingError{ this->device->errorMessage() };
}

void AudioDeviceController::fillStreamBuffer(void *channels, int frameCount) {
	reader->read(static_cast<float **>(channels), frameCount);
}
