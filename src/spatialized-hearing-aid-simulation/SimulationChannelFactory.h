#pragma once

#include "PrescriptionReader.h"
#include "BrirReader.h"
#include "SignalProcessor.h"
#include <common-includes/Interface.h>

class SimulationChannelFactory {
public:
	INTERFACE_OPERATIONS(SimulationChannelFactory);

	virtual std::shared_ptr<SignalProcessor> makeWithoutSimulation(
		float
	) = 0;

	struct Spatialization {
		BrirReader::impulse_response_type filterCoefficients;
	};
	virtual std::shared_ptr<SignalProcessor> makeSpatialization(
		Spatialization , float 
	) = 0;

	struct HearingAidSimulation {
		PrescriptionReader::Dsl prescription;
		double attack_ms;
		double release_ms;
		double fullScaleLevel_dB_Spl;
		int sampleRate;
		int windowSize;
		int chunkSize;
	};
	virtual std::shared_ptr<SignalProcessor> makeHearingAidSimulation(
		HearingAidSimulation , float 
	) = 0;

	struct FullSimulation {
		Spatialization spatialization;
		HearingAidSimulation hearingAid;
	};
	virtual std::shared_ptr<SignalProcessor> makeFullSimulation(
		FullSimulation , float 
	) = 0;
};