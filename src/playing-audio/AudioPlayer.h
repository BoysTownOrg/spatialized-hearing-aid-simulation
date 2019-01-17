#pragma once
#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <gsl/gsl>
#include <memory>
#include <string>
#include <vector>

class NoLongerFactory {
public:
	INTERFACE_OPERATIONS(NoLongerFactory);
	RUNTIME_ERROR(CreateError);
	struct Parameters {
		std::vector<double> channelScalars;
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioFilePath;
		double max_dB_Spl;
		double attack_ms;
		double release_ms;
		double level_dB_Spl;
		int chunkSize;
		int windowSize;
		int sampleRate;
		int channels;
	};
	virtual void make(Parameters) = 0;
	virtual bool complete() = 0;
	virtual void process(gsl::span<gsl::span<float>> audio) = 0;
	virtual int sampleRate() = 0;
	virtual int channels() = 0;
};

#include "AudioDevice.h"
#include <recognition-test/StimulusPlayer.h>
#include <gsl/gsl>

#ifdef PLAYING_AUDIO_EXPORTS
	#define PLAYING_AUDIO_API __declspec(dllexport)
#else
	#define PLAYING_AUDIO_API __declspec(dllimport)
#endif

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	NoLongerFactory::Parameters processing;
	AudioDevice *device;
	NoLongerFactory *noLongerAFactory;
public:
	PLAYING_AUDIO_API AudioPlayer(
		AudioDevice *, 
		NoLongerFactory *
	);
	void fillStreamBuffer(void * channels, int frames) override;
	PLAYING_AUDIO_API void play(PlayRequest) override;
	PLAYING_AUDIO_API std::vector<std::string> audioDeviceDescriptions() override;
	bool isPlaying() override;
	PLAYING_AUDIO_API void initialize(Initialization) override;
private:
	void makeProcessor(
		NoLongerFactory::Parameters p
	);
};

