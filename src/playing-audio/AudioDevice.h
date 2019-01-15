#pragma once

#include "AudioDeviceController.h"
#include <common-includes/Interface.h>
#include <vector>
#include <string>

class AudioDevice {
public:
	INTERFACE_OPERATIONS(AudioDevice);
	virtual void setController(AudioDeviceController *) = 0;
	virtual void startStream() = 0;
	virtual void stopStream() = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
	virtual bool streaming() const = 0;
	virtual void setCallbackResultToComplete() = 0;
	virtual void setCallbackResultToContinue() = 0;
	struct StreamParameters {
		unsigned long framesPerBuffer;
		int channels;
		int sampleRate;
		int deviceIndex;
	};
	virtual void openStream(StreamParameters) = 0;
	virtual void closeStream() = 0;
	virtual int count() = 0;
	virtual std::string description(int) = 0;
};
