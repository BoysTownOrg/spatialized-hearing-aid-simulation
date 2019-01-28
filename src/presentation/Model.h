#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <vector>

struct GlobalTestParameters;

class Model {
public:
	INTERFACE_OPERATIONS(Model);

	struct TestParameters {
		std::string audioDirectory;
		std::string testFilePath;
		GlobalTestParameters *global;
	};
	virtual void initializeTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	struct TrialParameters {
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void playTrial(TrialParameters) = 0;
	RUNTIME_ERROR(TrialFailure);

	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
	virtual bool testComplete() = 0;
	virtual void playCalibration() = 0;
	virtual void stopCalibration() = 0;
	virtual void calibrate(double level_dB_Spl) = 0;
};
