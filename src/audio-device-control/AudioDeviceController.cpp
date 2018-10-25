#include "AudioDeviceController.h"

AudioDeviceController::AudioDeviceController(
	std::shared_ptr<AudioDevice> device, 
	std::shared_ptr<AudioStream>
)
{
	device->setController(this);
}
