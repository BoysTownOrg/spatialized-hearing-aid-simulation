#pragma once

#include "PrescriptionReader.h"
#include "BrirReader.h"
#include "SignalProcessor.h"
#include <common-includes/Interface.h>

class ISpatializedHearingAidSimulationFactory {
public:
	INTERFACE_OPERATIONS(ISpatializedHearingAidSimulationFactory);
	struct SimulationParameters {
		PrescriptionReader::Dsl prescription;
		BrirReader::impulse_response_type filterCoefficients;
		double attack_ms;
		double release_ms;
		double fullScaleLevel_dB_Spl;
		float scale;
		int sampleRate;
		int windowSize;
		int chunkSize;
		bool usingHearingAidSimulation;
		bool usingSpatialization;
	};
	virtual std::shared_ptr<SignalProcessor> make(SimulationParameters p) = 0;
};