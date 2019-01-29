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

	virtual void playNextTrial() = 0;
	virtual std::string nextStimulus() = 0;
};

class RecognitionTest : public SpeechPerceptionTest_ {
	std::string nextStimulus_{};
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
	RECOGNITION_TEST_API void playNextTrial() override;
	RECOGNITION_TEST_API bool testComplete();
	std::string nextStimulus() override;
private:
	void prepareNewTest_(TestParameters);
	void initializeStimulusList(std::string directory);
	void initializeDocumenter(std::string testFilePath);
	void documentTestParameters(TestParameters);
};
