#pragma once

#include "AudioDevice.h"
#include "playing-audio-exports.h"
#include "RecognitionTestModel.h"
#include <audio-stream-processing/AudioFrameReader.h>

class AudioPlayer : public StimulusPlayer, public AudioDeviceController {
	AudioDevice *device;
	AudioFrameReaderFactory *readerFactory;
	std::shared_ptr<AudioFrameReader> frameReader{};
public:
	PLAYING_AUDIO_API AudioPlayer(AudioDevice *, AudioFrameReaderFactory *);
	void fillStreamBuffer(void * channels, int frames) override;
	void play(std::string filePath) override;
};

