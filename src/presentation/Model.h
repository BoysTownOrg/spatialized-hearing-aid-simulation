#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <vector>

class Model {
public:
	INTERFACE_OPERATIONS(Model);
	RUNTIME_ERROR(RequestFailure);

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
		ProcessingParameters processing;
		std::string subjectId;
		std::string testerId;
		std::string audioDirectory;
		std::string testFilePath;
	};
	virtual void prepareNewTest(TestParameters *) = 0;

	struct TrialParameters {
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void playNextTrial(TrialParameters *) = 0;

	virtual bool testComplete() = 0;

	struct CalibrationParameters {
		ProcessingParameters processing;
		std::string audioDevice;
		std::string audioFilePath;
		double level_dB_Spl;
	};
	virtual void playCalibration(CalibrationParameters *) = 0;
	virtual void stopCalibration() = 0;
	virtual std::vector<std::string> audioDeviceDescriptions() = 0;

	struct SaveAudioParameters {
		ProcessingParameters processing;
		std::string inputAudioFilePath;
		double level_dB_Spl;
	};
	virtual void processAudioForSaving(SaveAudioParameters *) = 0;
};
