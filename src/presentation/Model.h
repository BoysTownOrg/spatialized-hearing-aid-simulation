#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <vector>

class Model {
public:
	INTERFACE_OPERATIONS(Model);

	struct TestParameters {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioDirectory;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	virtual void initializeTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	struct TrialRequest {
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void playTrial(TrialRequest) = 0;
	RUNTIME_ERROR(RequestFailure);

	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
	virtual bool testComplete() = 0;
};
