#pragma once

#ifdef AUDIO_DEVICE_CONTROL_EXPORTS
	#define AUDIO_DEVICE_CONTROL_API __declspec(dllexport)
#else
	#define AUDIO_DEVICE_CONTROL_API __declspec(dllimport)
#endif

#include "AudioDevice.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <memory>

class AudioDeviceController {
	std::shared_ptr<AudioDevice> device;
	std::shared_ptr<AudioFrameReader> reader;
public:
	class DeviceConnectionFailure : public std::runtime_error {
	public:
		explicit DeviceConnectionFailure(std::string s) : std::runtime_error{ s } {}
	};
	AUDIO_DEVICE_CONTROL_API AudioDeviceController(
		std::shared_ptr<AudioDevice>,
		std::shared_ptr<AudioFrameReader>
	);
	AUDIO_DEVICE_CONTROL_API void startStreaming();
	AUDIO_DEVICE_CONTROL_API void stopStreaming();
	AUDIO_DEVICE_CONTROL_API void fillStreamBuffer(void *channels, int frameCount);
};

