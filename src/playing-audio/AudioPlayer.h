#pragma once

#include "AudioDevice.h"
#include "playing-audio-exports.h"
#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <recognition-test/RecognitionTestModel.h>
#include <gsl/gsl>

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	AudioDevice *device;
	AudioFrameReaderFactory *readerFactory;
	std::shared_ptr<AudioFrameReader> frameReader{};
	AudioFrameProcessorFactory *processorFactory;
	std::shared_ptr<AudioFrameProcessor> frameProcessor{};
public:
	PLAYING_AUDIO_API AudioPlayer(
		AudioDevice *, 
		AudioFrameReaderFactory *, 
		AudioFrameProcessorFactory *
	);
	void fillStreamBuffer(void * channels, int frames) override;
	void play(PlayRequest) override;
	PLAYING_AUDIO_API std::vector<std::string> audioDeviceDescriptions();
private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameProcessor> makeProcessor(
		AudioFrameProcessorFactory::Parameters p
	);
};

