#pragma once

#include "AudioDevice.h"
#include "playing-audio-exports.h"
#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <recognition-test/RecognitionTestModel.h>
#include <gsl/gsl>

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	std::shared_ptr<AudioFrameReader> frameReader{};
	std::shared_ptr<AudioFrameProcessor> frameProcessor{};
	AudioDevice *device;
	AudioFrameReaderFactory *readerFactory;
	AudioFrameProcessorFactory *processorFactory;
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

