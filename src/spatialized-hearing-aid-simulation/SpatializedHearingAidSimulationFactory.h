#pragma once

#include "ISpatializedHearingAidSimulationFactory.h"
#include "spatialized-hearing-aid-simulation-exports.h"
#include <hearing-aid-processing/FilterbankCompressor.h>

class FirFilterFactory {
public:
	INTERFACE_OPERATIONS(FirFilterFactory);
	virtual std::shared_ptr<SignalProcessor> make(BrirReader::impulse_response_type) = 0;
};

class HearingAidFactory {
public:
	INTERFACE_OPERATIONS(HearingAidFactory);
	virtual std::shared_ptr<SignalProcessor> make(
		FilterbankCompressor::Parameters
	) = 0;
};

class ScalarFactory {
public:
	INTERFACE_OPERATIONS(ScalarFactory);
	virtual std::shared_ptr<SignalProcessor> make(float) = 0;
};

class SpatializedHearingAidSimulationFactory :
	public ISpatializedHearingAidSimulationFactory
{
	ScalarFactory *scalarFactory;
	FirFilterFactory *firFilterFactory;
	HearingAidFactory *hearingAidFactory;
public:
	SPATIALIZED_HA_SIMULATION_API SpatializedHearingAidSimulationFactory(
		ScalarFactory *scalarFactory,
		FirFilterFactory *firFilterFactory,
		HearingAidFactory *hearingAidFactory
	);
	std::shared_ptr<SignalProcessor> make(SimulationParameters p) override;
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeFullSimulation(
		SimulationParameters p
	);
	struct ignroe {
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
	struct HearingAidSimulation {
		PrescriptionReader::Dsl prescription;
		double attack_ms;
		double release_ms;
		double fullScaleLevel_dB_Spl;
		int sampleRate;
		int windowSize;
		int chunkSize;
	};
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeHearingAidSimulation(
		HearingAidSimulation p, float scale
	);
	struct Spatialization {
		BrirReader::impulse_response_type filterCoefficients;
	};
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeSpatialization(
		Spatialization p, float scale
	);
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeWithoutSimulation(
		SimulationParameters p
	);
private:
	FilterbankCompressor::Parameters compression(SimulationParameters p);
};