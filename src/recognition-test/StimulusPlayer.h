#pragma once

#include <common-includes/RuntimeError.h>
#include <common-includes/Interface.h>
#include <vector>
#include <string>

class StimulusPlayer {
public:
	INTERFACE_OPERATIONS(StimulusPlayer);
	RUNTIME_ERROR(DeviceFailure);

	struct Initialization {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		double max_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	virtual void initialize(Initialization) = 0;
	RUNTIME_ERROR(InitializationFailure);

	struct PlayRequest {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioFilePath;
		std::string audioDevice;
		double max_dB_Spl;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	virtual void play(PlayRequest) = 0;
	RUNTIME_ERROR(RequestFailure);

	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
	virtual bool isPlaying() = 0;
};