#pragma once

#include "playing-audio-exports.h"
#include "AudioDevice.h"
#include "AudioFrameProcessor.h"
#include <audio-stream-processing/AudioFrameReader.h>
#include <audio-file-reading/AudioFileReader.h>
#include <presentation/Model.h>
#include <common-includes/RuntimeError.h>
#include <common-includes/Interface.h>

class StimulusList {
public:
	INTERFACE_OPERATIONS(StimulusList);
	virtual void initialize(std::string directory) = 0;
};

class StimulusPlayer {
public:
	INTERFACE_OPERATIONS(StimulusPlayer);
};

class RecognitionTestModel : public Model, public AudioDeviceController {
	std::vector<gsl::span<float>> audio;
	AudioDevice *device;
	AudioFrameReaderFactory *readerFactory;
	AudioFrameProcessorFactory *processorFactory;
	std::shared_ptr<AudioFrameReader> frameReader{};
	std::shared_ptr<AudioFrameProcessor> frameProcessor{};
	StimulusList *list;
public:
	RUNTIME_ERROR(DeviceFailure);
	PLAYING_AUDIO_API RecognitionTestModel(
		AudioDevice *device,
		AudioFrameReaderFactory *readerFactory,
		AudioFrameProcessorFactory *processorFactory,
		StimulusList *list,
		StimulusPlayer *player
	);
	PLAYING_AUDIO_API void play(PlayRequest) override;
	void fillStreamBuffer(void *channels, int frames) override;
	PLAYING_AUDIO_API void initializeTest(TestParameters) override;
	PLAYING_AUDIO_API void playTrial(PlayRequest) override;
	std::vector<std::string> audioDeviceDescriptions() override;
private:
	std::shared_ptr<AudioFrameReader> makeReader(std::string filePath);
	std::shared_ptr<AudioFrameProcessor> makeProcessor(AudioFrameProcessorFactory::Parameters p);
};
