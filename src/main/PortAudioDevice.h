#pragma once

#include <audio-device-control/AudioDeviceController.h>

class PortAudioDevice : public AudioDevice {
	AudioDeviceController *controller{};
public:
	void setController(AudioDeviceController *) override;
	void startStream() override;
	void stopStream() override;
};
