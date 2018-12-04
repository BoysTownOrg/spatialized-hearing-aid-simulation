#pragma once

#include <common-includes/Interface.h>

class AudioDeviceController {
public:
	INTERFACE_OPERATIONS(AudioDeviceController);
	virtual void fillStreamBuffer(void *channels, int frameCount) = 0;
};
