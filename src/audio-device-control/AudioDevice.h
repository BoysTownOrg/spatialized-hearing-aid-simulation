#pragma once

#include <common-includes/Interface.h>
#include <vector>
#include <string>

class AudioDeviceController;

class AudioDevice {
public:
	struct Parameters {
		std::vector<int> channels;
		unsigned long framesPerBuffer;
		int sampleRate;
	};
	INTERFACE_OPERATIONS(AudioDevice);
	virtual void setController(AudioDeviceController *) = 0;
	virtual void startStream() = 0;
	virtual void stopStream() = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
};
