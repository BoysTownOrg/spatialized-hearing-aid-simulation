#pragma once

#include <common-includes/Interface.h>
#include <string>

class SpatializedHearingAidSimulator {
public:
	struct Parameters {
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
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulator);
};

#include <memory>

class SpatializedHearingAidSimulatorFactory {
public:
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulatorFactory);
	virtual std::shared_ptr<SpatializedHearingAidSimulator> make(
		SpatializedHearingAidSimulator::Parameters) = 0;
};

