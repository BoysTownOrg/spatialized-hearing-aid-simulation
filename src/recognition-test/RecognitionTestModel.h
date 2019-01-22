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

class RecognitionTestModel : public Model {
	std::string currentStimulus_{};
	StimulusList *list;
	StimulusPlayer *player;
	Documenter *documenter;
	bool failedOnLastPlayRequest{};
public:
	RECOGNITION_TEST_API RecognitionTestModel(
		StimulusList *,
		StimulusPlayer *,
		Documenter *
	);
	RECOGNITION_TEST_API void initializeTest(TestParameters) override;
	RECOGNITION_TEST_API void playTrial(TrialParameters) override;
	std::vector<std::string> audioDeviceDescriptions() override;
	std::vector<int> preferredProcessingSizes() override;
	bool testComplete() override;
private:
	void initializeTest_(TestParameters);
	void initializeStimulusPlayer(TestParameters);
	void initializeStimulusList(std::string directory);
	void initializeDocumenter(std::string testFilePath);
	void documentTestParameters(TestParameters);
	void playTrial_(TrialParameters);
	void playNextStimulus(TrialParameters);
};
