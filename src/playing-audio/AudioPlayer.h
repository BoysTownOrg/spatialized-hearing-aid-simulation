#pragma once

#include "AudioDevice.h"
#include "AudioLoader.h"
#include "playing-audio-exports.h"
#include <recognition-test/StimulusPlayer.h>

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
};

class AudioPlayer : public StimulusPlayer, public AudioDeviceController, public IAudioPlayer {
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
	void setAudioLoader(AudioLoader *);
	void fillStreamBuffer(void * channels, int frames) override;
	bool isPlaying() override;
	void stop() override;
	void play() override;
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

