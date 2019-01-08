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
	virtual std::string next() = 0;
	virtual bool empty() = 0;
};

class StimulusPlayer {
public:
	INTERFACE_OPERATIONS(StimulusPlayer);
	RUNTIME_ERROR(RequestFailure);
	struct PlayRequest {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioFilePath;
		std::string audioDevice;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	virtual void play(PlayRequest) = 0;
};

class RecognitionTestModel : public Model {
	std::vector<gsl::span<float>> audio;
	AudioDevice *device;
	AudioFrameReaderFactory *readerFactory;
	AudioFrameProcessorFactory *processorFactory;
	std::shared_ptr<AudioFrameReader> frameReader{};
	std::shared_ptr<AudioFrameProcessor> frameProcessor{};
	StimulusList *list;
	StimulusPlayer *player;
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
	PLAYING_AUDIO_API void initializeTest(TestParameters) override;
	PLAYING_AUDIO_API void playTrial(PlayRequest) override;
	std::vector<std::string> audioDeviceDescriptions() override;
	bool testComplete() override;
};
