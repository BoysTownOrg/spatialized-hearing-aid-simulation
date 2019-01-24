#pragma once

#include <common-includes/Interface.h>
#include <string>

struct GlobalTestParameters;
struct GlobalTrialParameters;

class Documenter {
public:
	INTERFACE_OPERATIONS(Documenter);
	virtual void initialize(std::string filePath) = 0;

	struct TestParameters{
		GlobalTestParameters *global;
	};
	virtual void documentTestParameters(TestParameters) = 0;
	
	struct TrialParameters{
		GlobalTrialParameters *global;
	};

	virtual void write(std::string) = 0;
	virtual bool failed() = 0;
	virtual std::string errorMessage() = 0;
};