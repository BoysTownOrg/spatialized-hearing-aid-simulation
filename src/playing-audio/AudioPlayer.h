#pragma once

#include "AudioDevice.h"
#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <recognition-test/StimulusPlayer.h>
#include <gsl/gsl>

#ifdef PLAYING_AUDIO_EXPORTS
	#define PLAYING_AUDIO_API __declspec(dllexport)
#else
	#define PLAYING_AUDIO_API __declspec(dllimport)
#endif

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	std::shared_ptr<AudioFrameReader> frameReader{};
	std::shared_ptr<AudioFrameProcessor> frameProcessor{};
	AudioDevice *device;
	AudioFrameReaderFactory *readerFactory;
	AudioFrameProcessorFactory *processorFactory;
	int paddedZeroes{};
public:
	PLAYING_AUDIO_API AudioPlayer(
		AudioDevice *, 
		AudioFrameReaderFactory *, 
		AudioFrameProcessorFactory *
	);
	void fillStreamBuffer(void * channels, int frames) override;
	PLAYING_AUDIO_API void play(PlayRequest) override;
	PLAYING_AUDIO_API std::vector<std::string> audioDeviceDescriptions() override;
	bool isPlaying() override;
private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameProcessor> makeProcessor(
		AudioFrameProcessorFactory::Parameters p
	);
};

