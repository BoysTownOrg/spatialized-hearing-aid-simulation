#include "SignalProcessingChain.h"
#include "SpatializedHearingAidSimulationFactory.h"

SpatializedHearingAidSimulationFactory::SpatializedHearingAidSimulationFactory(
	ScalarFactory *scalarFactory,
	FirFilterFactory *firFilterFactory,
	HearingAidFactory *hearingAidFactory
) :
	scalarFactory{ scalarFactory },
	firFilterFactory{ firFilterFactory },
	hearingAidFactory{ hearingAidFactory } {}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeFullSimulation(
	FullSimulation p, 
	float scale
) {
	auto chain = std::make_shared<SignalProcessingChain>();
	chain->add(scalarFactory->make(scale));
	chain->add(firFilterFactory->make(std::move(p.spatialization.filterCoefficients)));
	chain->add(hearingAidFactory->make(compression(std::move(p.hearingAid))));
	return chain;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeHearingAidSimulation(
	HearingAidSimulation p, 
	float scale
)
{
	auto chain = std::make_shared<SignalProcessingChain>();
	chain->add(scalarFactory->make(scale));
	chain->add(hearingAidFactory->make(compression(std::move(p))));
	return chain;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeSpatialization(
	Spatialization p, 
	float scale
) {
	auto chain = std::make_shared<SignalProcessingChain>();
	chain->add(scalarFactory->make(scale));
	chain->add(firFilterFactory->make(std::move(p.filterCoefficients)));
	return chain;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeWithoutSimulation(
	float scale
) {
	return scalarFactory->make(scale);
}

FilterbankCompressor::Parameters SpatializedHearingAidSimulationFactory::compression(
	HearingAidSimulation p
) {
	FilterbankCompressor::Parameters compression_;
	compression_.compressionRatios = std::move(p.prescription.compressionRatios);
	compression_.crossFrequenciesHz = std::move(p.prescription.crossFrequenciesHz);
	compression_.kneepointGains_dB = std::move(p.prescription.kneepointGains_dB);
	compression_.kneepoints_dBSpl = std::move(p.prescription.kneepoints_dBSpl);
	compression_.broadbandOutputLimitingThresholds_dBSpl =
		std::move(p.prescription.broadbandOutputLimitingThresholds_dBSpl);
	compression_.channels = p.prescription.channels;
	compression_.attack_ms = p.attack_ms;
	compression_.release_ms = p.release_ms;
	compression_.chunkSize = p.chunkSize;
	compression_.windowSize = p.windowSize;
	compression_.sampleRate = p.sampleRate;
	compression_.max_dB_Spl = p.fullScaleLevel_dB_Spl;
	return compression_;
}
