#pragma once

#include "AudioDevice.h"
#include "AudioProcessor.h"
#include "playing-audio-exports.h"
#include <recognition-test/StimulusPlayer.h>

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	int framesPerBuffer_{};
	AudioDevice *device;
	AudioProcessor *processor;
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
	std::vector<int> preferredProcessingSizes() override;
private:
	void initializeProcessor(Initialization);
	void play_(PlayRequest);
	void startStream(PlayRequest);
	void prepareProcessor(PlayRequest);
};

