#pragma once

#include <dsl-prescription/PrescriptionReader.h>
#include <binaural-room-impulse-response/BrirReader.h>
#include <signal-processing/SignalProcessor.h>
#include <common-includes/Interface.h>

class IRefactoredSpatializedHearingAidSimulationFactory {
public:
	INTERFACE_OPERATIONS(IRefactoredSpatializedHearingAidSimulationFactory);
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