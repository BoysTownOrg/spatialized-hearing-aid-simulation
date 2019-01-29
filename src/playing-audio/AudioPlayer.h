#pragma once

#include "AudioDevice.h"
#include "AudioLoader.h"
#include "playing-audio-exports.h"
#include <recognition-test/StimulusPlayer.h>

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	AudioDevice *device;
	AudioLoader *loader;
public:
	PLAYING_AUDIO_API AudioPlayer(
		AudioDevice *, 
		AudioLoader *
	);
	PLAYING_AUDIO_API void prepareToPlay(Preparation) override;
	PLAYING_AUDIO_API std::vector<std::string> audioDeviceDescriptions() override;
	void fillStreamBuffer(void * channels, int frames) override;
	bool isPlaying() override;
	void stop() override;
	void play() override;
private:
	void play_(Preparation);
	void reopenStream(std::string deviceName);
	template<typename exception>
		void throwIfDeviceFailed();
	void openStream(std::string deviceName);
	void prepareLoader(AudioLoader::Preparation);
	void prepareAudioForLoading(void * channels, int frames);
	void signalDeviceIfDoneLoading();
	int findDeviceIndex(std::string deviceName);
};

