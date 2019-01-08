#pragma once

#include "playing-audio-exports.h"
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
	RUNTIME_ERROR(DeviceFailure);
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
	StimulusList *list;
	StimulusPlayer *player;
public:
	PLAYING_AUDIO_API RecognitionTestModel(
		StimulusList *list,
		StimulusPlayer *player
	);
	PLAYING_AUDIO_API void initializeTest(TestParameters) override;
	PLAYING_AUDIO_API void playTrial(TrialRequest) override;
	std::vector<std::string> audioDeviceDescriptions() override;
	bool testComplete() override;
};
