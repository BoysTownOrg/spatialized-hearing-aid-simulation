#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <vector>

class Model {
public:
	INTERFACE_OPERATIONS(Model);

	struct ProcessingParameters {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
		bool usingHearingAidSimulation;
		bool usingSpatialization;
	};

	struct TestParameters {
		std::string subjectId;
		std::string testerId;
		std::string audioDirectory;
		std::string testFilePath;
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
		bool usingHearingAidSimulation;
		bool usingSpatialization;
	};
	virtual void prepareNewTest(TestParameters) = 0;
	RUNTIME_ERROR(TestInitializationFailure);

	struct TrialParameters {
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void playTrial(TrialParameters) = 0;
	RUNTIME_ERROR(TrialFailure);

	virtual bool testComplete() = 0;

	struct CalibrationParameters {
		ProcessingParameters processing;
		std::string audioDevice;
		std::string audioFilePath;
		double level_dB_Spl;
	};
	virtual void playCalibration(CalibrationParameters) = 0;
	RUNTIME_ERROR(CalibrationFailure);
	virtual void stopCalibration() = 0;
	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
};
