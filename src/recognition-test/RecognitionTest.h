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

class RecognitionTest : public Model {
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
	RECOGNITION_TEST_API void playTrial(TrialParameters) override;
	bool testComplete() override;
	void playCalibration(CalibrationParameters) override;
	void stopCalibration() override;
private:
	void prepareNewTest_(TestParameters);
	void initializeStimulusList(std::string directory);
	void initializeDocumenter(std::string testFilePath);
	void documentTestParameters(TestParameters);
	void documentTrialParameters(TrialParameters);
	void playCalibration_(CalibrationParameters);
};
