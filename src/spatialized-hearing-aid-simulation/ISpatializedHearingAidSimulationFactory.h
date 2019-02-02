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
	virtual std::shared_ptr<SignalProcessor> make(SimulationParameters) { return {}; };

	struct Spatialization {
		BrirReader::impulse_response_type filterCoefficients;
	};
	struct HearingAidSimulation {
		PrescriptionReader::Dsl prescription;
		double attack_ms;
		double release_ms;
		double fullScaleLevel_dB_Spl;
		int sampleRate;
		int windowSize;
		int chunkSize;
	};
	struct FullSimulation {
		Spatialization spatialization;
		HearingAidSimulation hearingAidSimulation;
	};
	virtual std::shared_ptr<SignalProcessor> makeFullSimulation(
		FullSimulation , float 
	) {
		return {};
	}
	virtual std::shared_ptr<SignalProcessor> makeHearingAidSimulation(
		HearingAidSimulation , float 
	) {
		return {};
	}
	virtual std::shared_ptr<SignalProcessor> makeSpatialization(
		Spatialization , float 
	) {
		return {};
	}
	virtual std::shared_ptr<SignalProcessor> makeWithoutSimulation(
		float 
	) {
		return {};
	}
};