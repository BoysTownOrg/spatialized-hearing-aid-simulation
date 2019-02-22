#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <vector>

class Model {
public:
	INTERFACE_OPERATIONS(Model);
	RUNTIME_ERROR(RequestFailure);

	struct Processing {
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

	struct Testing {
		Processing processing;
		std::string subjectId;
		std::string testerId;
		std::string audioDirectory;
		std::string testFilePath;
	};
	virtual void prepareNewTest(Testing *) = 0;

	struct Trial {
		std::string audioDevice;
		double level_dB_Spl;
	};
	virtual void playNextTrial(Trial *) = 0;

	virtual bool testComplete() = 0;

	struct Calibration {
		Processing processing;
		std::string audioDevice;
		std::string audioFilePath;
		double level_dB_Spl;
	};
	virtual void playCalibration(Calibration *) = 0;
	virtual void stopCalibration() = 0;
	virtual std::vector<std::string> audioDeviceDescriptions() = 0;

	struct SavingAudio {
		Processing processing;
		std::string inputAudioFilePath;
		double level_dB_Spl;
	};
	virtual void processAudioForSaving(SavingAudio *) = 0;
	virtual void saveAudio(std::string filePath) = 0;
};
