#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

struct GlobalTestParameters;
struct GlobalTrialParameters;

class Documenter {
public:
	INTERFACE_OPERATIONS(Documenter);
	virtual void initialize(std::string filePath) = 0;
	RUNTIME_ERROR(InitializationFailure);

	struct TestParameters{
		GlobalTestParameters *global;
	};
	virtual void documentTestParameters(TestParameters) = 0;
	
	struct TrialParameters{
		GlobalTrialParameters *global;
	};
	virtual void documentTrialParameters(TrialParameters) = 0;
};