#pragma once

#ifdef AUDIO_DEVICE_CONTROL_EXPORTS
	#define AUDIO_DEVICE_CONTROL_API __declspec(dllexport)
#else
	#define AUDIO_DEVICE_CONTROL_API __declspec(dllimport)
#endif

#include "AudioDevice.h"
#include "AudioStream.h"
#include <memory>

class AudioDeviceController {
	std::shared_ptr<AudioDevice> device;
	std::shared_ptr<AudioStream> stream;
public:
	AUDIO_DEVICE_CONTROL_API AudioDeviceController(
		std::shared_ptr<AudioDevice> device,
		std::shared_ptr<AudioStream> stream
	);
	AUDIO_DEVICE_CONTROL_API void startStreaming();
	AUDIO_DEVICE_CONTROL_API void stopStreaming();
	AUDIO_DEVICE_CONTROL_API void fillStreamBuffer(void *channels, int frameCount);
};

