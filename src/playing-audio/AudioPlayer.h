#pragma once

#include "AudioDevice.h"
#include "AudioProcessor.h"
#include "playing-audio-exports.h"
#include <recognition-test/StimulusPlayer.h>

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	AudioProcessor::Initialization processing;
	AudioDevice *device;
	AudioProcessor *noLongerAFactory;
public:
	PLAYING_AUDIO_API AudioPlayer(
		AudioDevice *, 
		AudioProcessor *
	);
	void fillStreamBuffer(void * channels, int frames) override;
	PLAYING_AUDIO_API void play(PlayRequest) override;
	PLAYING_AUDIO_API std::vector<std::string> audioDeviceDescriptions() override;
	bool isPlaying() override;
	PLAYING_AUDIO_API void initialize(Initialization) override;
};

