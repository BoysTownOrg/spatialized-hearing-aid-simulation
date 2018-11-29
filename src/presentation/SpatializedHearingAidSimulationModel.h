#pragma once

#include <common-includes/Interface.h>
#include <common-includes/RuntimeError.h>
#include <string>

class SpatializedHearingAidSimulationModel {
public:
	RUNTIME_ERROR(RequestFailure);
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulationModel);
	struct PlayRequest {
		std::string leftDslPrescriptionFilePath;
		std::string rightDslPrescriptionFilePath;
		std::string brirFilePath;
		std::string audioFilePath;
		double level_dB_Spl;
		double attack_ms;
		double release_ms;
		int windowSize;
		int chunkSize;
	};
	virtual void playRequest(PlayRequest) = 0;
};
