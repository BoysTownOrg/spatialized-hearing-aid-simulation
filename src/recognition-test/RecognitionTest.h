#pragma once

#include "StimulusList.h"
#include "StimulusPlayer.h"
#include "Documenter.h"
#include <presentation/Model.h>

#ifdef RECOGNITION_TEST_EXPORTS
	#define RECOGNITION_TEST_API __declspec(dllexport)
#else
	#define RECOGNITION_TEST_API __declspec(dllimport)
#endif

struct GlobalTrialParameters {
	std::string stimulus;
	double level_dB_Spl;
};

class SpeechPerceptionTest_ {
public:
	INTERFACE_OPERATIONS(SpeechPerceptionTest_);
	struct TestParameters {
		std::string audioDirectory;
		std::string testFilePath;
	};
	virtual void prepareNewTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	struct TrialParameters {
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void prepareNextTrial(TrialParameters) = 0;
	virtual void playTrial() = 0;
};

class RecognitionTest : public SpeechPerceptionTest_ {
	std::string currentStimulus_{};
	StimulusList *list;
	StimulusPlayer *player;
	Documenter *documenter;
	bool failedOnLastPlayRequest{};
public:
	RECOGNITION_TEST_API RecognitionTest(
		StimulusList *,
		StimulusPlayer *,
		Documenter *
	);
	RECOGNITION_TEST_API void prepareNewTest(TestParameters) override;
	void prepareNextTrial(TrialParameters) override;
	RECOGNITION_TEST_API void playTrial() override;
	RECOGNITION_TEST_API bool testComplete();
private:
	void prepareNewTest_(TestParameters);
	void initializeStimulusList(std::string directory);
	void initializeDocumenter(std::string testFilePath);
	void documentTestParameters(TestParameters);
	void documentTrialParameters(TrialParameters);
};
