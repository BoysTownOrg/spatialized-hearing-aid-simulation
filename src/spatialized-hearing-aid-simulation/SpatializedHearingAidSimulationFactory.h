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
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeFullSimulation(
		FullSimulation p, float scale
	) override;
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeHearingAidSimulation(
		HearingAidSimulation p, float scale
	) override;
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeSpatialization(
		Spatialization p, float scale
	) override;
	SPATIALIZED_HA_SIMULATION_API std::shared_ptr<SignalProcessor> makeWithoutSimulation(
		float scale
	) override;
private:
	std::shared_ptr<SignalProcessor> makeScalingProcessor(float scale);
	std::shared_ptr<SignalProcessor> makeFirFilter(Spatialization);
	FilterbankCompressor::Parameters compression(HearingAidSimulation p);
};