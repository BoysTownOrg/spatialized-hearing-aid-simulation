#pragma once

#include <recognition-test/StimulusPlayer.h>
#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

class SpeechPerceptionTest {
public:
	INTERFACE_OPERATIONS(SpeechPerceptionTest);
	struct TestParameters {
		std::string subjectId;
		std::string testerId;
		std::string audioDirectory;
		std::string testFilePath;
	};
	virtual void prepareNewTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	virtual void playNextTrial(StimulusPlayer *) = 0;
	virtual std::string nextStimulus() = 0;
	virtual bool testComplete() = 0;
};