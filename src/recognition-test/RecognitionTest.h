#pragma once

#include "StimulusList.h"
#include "Documenter.h"
#include <spatialized-hearing-aid-simulation/SpeechPerceptionTest.h>
#include <presentation/Model.h>

#ifdef RECOGNITION_TEST_EXPORTS
	#define RECOGNITION_TEST_API __declspec(dllexport)
#else
	#define RECOGNITION_TEST_API __declspec(dllimport)
#endif

class RecognitionTest : public SpeechPerceptionTest {
	std::string nextStimulus_{};
	StimulusList *list;
	Documenter *documenter;
public:
	RECOGNITION_TEST_API RecognitionTest(
		StimulusList *,
		Documenter *
	);
	RECOGNITION_TEST_API void prepareNewTest(TestParameters) override;
	RECOGNITION_TEST_API void advanceTrial() override;
	RECOGNITION_TEST_API bool testComplete() override;
	std::string nextStimulus() override;
private:
	void prepareNewTest_(TestParameters);
	void initializeStimulusList(std::string directory);
	void initializeDocumenter(std::string testFilePath);
	void documentTestParameters(TestParameters);
};
