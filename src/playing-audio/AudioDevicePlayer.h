#pragma once

#include "AudioDevice.h"
#include "playing-audio-exports.h"
#include <spatialized-hearing-aid-simulation/AudioLoader.h>
#include <spatialized-hearing-aid-simulation/AudioPlayer.h>

class AudioDevicePlayer : public AudioDeviceController, public AudioPlayer {
	std::vector<AudioLoader::channel_type> audio;
	std::shared_ptr<AudioLoader> loader{};
	AudioDevice *device;
public:
	explicit PLAYING_AUDIO_API AudioDevicePlayer(AudioDevice *);
	PLAYING_AUDIO_API void prepareToPlay(Preparation) override;
	PLAYING_AUDIO_API std::vector<std::string> audioDeviceDescriptions() override;
	PLAYING_AUDIO_API void setAudioLoader(std::shared_ptr<AudioLoader>) override;
	PLAYING_AUDIO_API void fillStreamBuffer(void * channels, int frames) override;
	PLAYING_AUDIO_API bool isPlaying() override;
	PLAYING_AUDIO_API void stop() override;
	PLAYING_AUDIO_API void play() override;
private:
	void prepareToPlay_(Preparation);
	void reopenStream(Preparation);
	template<typename exception>
		void throwIfDeviceFailed();
	void openStream(Preparation);
	void prepareAudioForLoading(void * channels, int frames);
	void signalDeviceIfDoneLoading();
	int findDeviceIndex(std::string deviceName);
};

