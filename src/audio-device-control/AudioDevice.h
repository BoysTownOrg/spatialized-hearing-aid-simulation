#pragma once

#include <common-includes/Interface.h>

class AudioDeviceController;

class AudioDevice {
public:
	INTERFACE_OPERATIONS(AudioDevice);
	virtual void setController(AudioDeviceController *) = 0;
};
