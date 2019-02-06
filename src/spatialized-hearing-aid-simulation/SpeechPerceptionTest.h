#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

class SpeechPerceptionTest {
public:
	INTERFACE_OPERATIONS(SpeechPerceptionTest);
	struct TestParameters {
		std::string subjectId;
		std::string testerId;
		std::string stimulusList;
		std::string outputFilePath;
	};
	virtual void prepareNewTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	virtual void advanceTrial() = 0;
	virtual std::string nextStimulus() = 0;
	virtual bool testComplete() = 0;
};