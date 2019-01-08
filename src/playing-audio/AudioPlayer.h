#pragma once

#include "AudioDevice.h"
#include "playing-audio-exports.h"
#include "RecognitionTestModel.h"

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	AudioDevice *device;
public:
	PLAYING_AUDIO_API explicit AudioPlayer(AudioDevice *device);
	void fillStreamBuffer(void * channels, int frames) override;
	void play(std::string filePath) override;
};

