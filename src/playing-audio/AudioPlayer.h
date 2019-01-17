#pragma once

#include "AudioDevice.h"
#include "AudioProcessor.h"
#include <recognition-test/StimulusPlayer.h>

#ifdef PLAYING_AUDIO_EXPORTS
	#define PLAYING_AUDIO_API __declspec(dllexport)
#else
	#define PLAYING_AUDIO_API __declspec(dllimport)
#endif

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

