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
	chain->add(makeScalingProcessor(scale));
	chain->add(makeFirFilter(std::move(p.spatialization)));
	chain->add(makeHearingAid(std::move(p.hearingAid)));
	return chain;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeScalingProcessor(float scale) {
	return scalarFactory->make(scale);
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeFirFilter(Spatialization s) {
	return firFilterFactory->make(std::move(s.filterCoefficients));
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeHearingAid(HearingAidSimulation s) {
	return hearingAidFactory->make(compression(std::move(s)));
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeHearingAidSimulation(
	HearingAidSimulation s, 
	float scale
)
{
	auto chain = std::make_shared<SignalProcessingChain>();
	chain->add(makeScalingProcessor(scale));
	chain->add(makeHearingAid(std::move(s)));
	return chain;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeSpatialization(
	Spatialization s, 
	float scale
) {
	auto chain = std::make_shared<SignalProcessingChain>();
	chain->add(makeScalingProcessor(scale));
	chain->add(makeFirFilter(std::move(s)));
	return chain;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeWithoutSimulation(
	float scale
) {
	return makeScalingProcessor(scale);
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
