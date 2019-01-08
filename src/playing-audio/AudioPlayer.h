#pragma once

#include "AudioDevice.h"
#include "playing-audio-exports.h"

class AudioPlayer : public AudioDeviceController {
public:
	PLAYING_AUDIO_API AudioPlayer(AudioDevice *device);
	void fillStreamBuffer(void * channels, int frames) override;
};

