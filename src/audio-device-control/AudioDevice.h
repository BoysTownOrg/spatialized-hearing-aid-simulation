#pragma once

#include <common-includes/Interface.h>

class AudioDeviceController;

class AudioDevice {
public:
	INTERFACE_OPERATIONS(AudioDevice);
	virtual void setController(AudioDeviceController *) = 0;
	virtual void startStream() = 0;
	virtual void stopStream() = 0;
};
