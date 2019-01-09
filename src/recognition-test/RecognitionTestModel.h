#pragma once

#ifdef RECOGNITION_TEST_EXPORTS
	#define RECOGNITION_TEST_API __declspec(dllexport)
#else
	#define RECOGNITION_TEST_API __declspec(dllimport)
#endif

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
	RUNTIME_ERROR(RequestFailure);

	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
};

class RecognitionTestModel : public Model {
	TestParameters forTest{};
	StimulusList *list;
	StimulusPlayer *player;
public:
	RECOGNITION_TEST_API RecognitionTestModel(
		StimulusList *list,
		StimulusPlayer *player
	);
	RECOGNITION_TEST_API void initializeTest(TestParameters) override;
	RECOGNITION_TEST_API void playTrial(TrialParameters) override;
	std::vector<std::string> audioDeviceDescriptions() override;
	bool testComplete() override;
};
