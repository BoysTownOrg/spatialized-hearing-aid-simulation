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

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::make(
	SimulationParameters p
) {
	auto chain = std::make_shared<SignalProcessingChain>();
	chain->add(scalarFactory->make(p.scale));
	if (p.usingSpatialization)
		chain->add(firFilterFactory->make(p.filterCoefficients));
	if (p.usingHearingAidSimulation)
		chain->add(hearingAidFactory->make(compression(p)));
	return chain;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeFullSimulation(SimulationParameters p)
{
	p.usingHearingAidSimulation = true;
	p.usingSpatialization = true;
	return make(std::move(p));
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeHearingAidSimulation(SimulationParameters p)
{
	p.usingHearingAidSimulation = true;
	p.usingSpatialization = false;
	return make(std::move(p));
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeSpatialization(SimulationParameters p)
{
	p.usingHearingAidSimulation = true;
	p.usingSpatialization = false;
	return make(std::move(p));
}

FilterbankCompressor::Parameters SpatializedHearingAidSimulationFactory::compression(
	SimulationParameters p
) {
	FilterbankCompressor::Parameters compression_;
	compression_.compressionRatios = p.prescription.compressionRatios;
	compression_.crossFrequenciesHz = p.prescription.crossFrequenciesHz;
	compression_.kneepointGains_dB = p.prescription.kneepointGains_dB;
	compression_.kneepoints_dBSpl = p.prescription.kneepoints_dBSpl;
	compression_.broadbandOutputLimitingThresholds_dBSpl =
		p.prescription.broadbandOutputLimitingThresholds_dBSpl;
	compression_.channels = p.prescription.channels;
	compression_.attack_ms = p.attack_ms;
	compression_.release_ms = p.release_ms;
	compression_.chunkSize = p.chunkSize;
	compression_.windowSize = p.windowSize;
	compression_.sampleRate = p.sampleRate;
	compression_.max_dB_Spl = p.fullScaleLevel_dB_Spl;
	return compression_;
}
