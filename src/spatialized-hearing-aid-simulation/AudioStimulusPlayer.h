#pragma once

#include <recognition-test/StimulusPlayer.h>
#include <playing-audio/AudioLoader.h>
#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <vector>
#include <string>

class IAudioPlayer {
public:
	INTERFACE_OPERATIONS(IAudioPlayer);
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
	virtual void setAudioLoader(AudioLoader *) = 0;
};

class AudioStimulusPlayer : public IAudioPlayer, public StimulusPlayer {};