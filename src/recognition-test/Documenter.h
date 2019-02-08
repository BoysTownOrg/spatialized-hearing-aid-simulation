#pragma once

#include <presentation/Model.h>
#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

class Documenter {
public:
	INTERFACE_OPERATIONS(Documenter);
	virtual void initialize(std::string filePath) = 0;
	RUNTIME_ERROR(InitializationFailure);

	virtual void documentTestParameters(Model::TestParameters) = 0;
	
	struct TrialParameters{
		std::string stimulus;
		double level_dB_Spl;
	};
	virtual void documentTrialParameters(TrialParameters) = 0;
};