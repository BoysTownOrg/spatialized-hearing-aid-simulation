#pragma once

#include <common-includes/Interface.h>

class AudioDeviceController;

class AudioDevice {
public:
	INTERFACE_OPERATIONS(AudioDevice);
	virtual void setController(AudioDeviceController *) = 0;
};

class AudioStream {};

#include <memory>

class AudioDeviceController {
public:
	AudioDeviceController(
		std::shared_ptr<AudioDevice> device,
		std::shared_ptr<AudioStream>
	)
	{
		device->setController(this);
	}
};

