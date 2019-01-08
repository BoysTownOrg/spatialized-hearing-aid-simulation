#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <vector>

class Model {
public:
	INTERFACE_OPERATIONS(Model);
	RUNTIME_ERROR(RequestFailure);
	struct PlayRequest {
		std::string audioFilePath;
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void play(PlayRequest) = 0;

	struct TestParameters {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioDirectory;
		std::string audioDevice;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	RUNTIME_ERROR(TestInitializationFailure);
	virtual void initializeTest(TestParameters) = 0;
	virtual void playTrial(PlayRequest) = 0;
	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
};
