#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>
#include <vector>

class SpatializedHearingAidSimulationModel {
public:
	RUNTIME_ERROR(RequestFailure);
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulationModel);
	virtual std::vector<std::string> audioDeviceDescriptions() = 0;
	struct PlayRequest {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioFilePath;
		std::string audioDevice;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	virtual void playRequest(PlayRequest) = 0;
};
