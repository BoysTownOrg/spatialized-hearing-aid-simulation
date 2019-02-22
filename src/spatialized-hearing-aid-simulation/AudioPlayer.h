#pragma once

#include "AudioLoader.h"
#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>
#include <string>
#include <memory>

class AudioPlayer {
public:
	INTERFACE_OPERATIONS(AudioPlayer);
	RUNTIME_ERROR(DeviceFailure);

	struct Preparation {
		std::string audioDevice;
		int framesPerBuffer;
		int channels = 0;
		int sampleRate;
	};
	virtual void prepareToPlay(Preparation) = 0;
	RUNTIME_ERROR(PreparationFailure);
	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
	virtual void setAudioLoader(std::shared_ptr<AudioLoader>) = 0;
	virtual void play() = 0;
	virtual void stop() = 0;
	virtual bool isPlaying() = 0;
};
