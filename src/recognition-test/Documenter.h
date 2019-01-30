#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

class Documenter {
public:
	INTERFACE_OPERATIONS(Documenter);
	virtual void initialize(std::string filePath) = 0;
	RUNTIME_ERROR(InitializationFailure);

	struct TestParameters{
		std::string subjectId;
		std::string testerId;
	};
	virtual void documentTestParameters(TestParameters) = 0;
	
	struct TrialParameters{
		std::string stimulus;
	};
	virtual void documentTrialParameters(TrialParameters) = 0;
};