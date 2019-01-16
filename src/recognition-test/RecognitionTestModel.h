#pragma once

#include "StimulusList.h"
#include "StimulusPlayer.h"
#include <presentation/Model.h>

#ifdef RECOGNITION_TEST_EXPORTS
	#define RECOGNITION_TEST_API __declspec(dllexport)
#else
	#define RECOGNITION_TEST_API __declspec(dllimport)
#endif

class RecognitionTestModel : public Model {
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
