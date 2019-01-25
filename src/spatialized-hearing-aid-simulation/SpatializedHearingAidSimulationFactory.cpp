#include "SpatializedHearingAidSimulationFactory.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ScalingProcessor.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <gsl/gsl>

SpatializedHearingAidSimulationFactory::SpatializedHearingAidSimulationFactory(
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
	std::shared_ptr<PrescriptionReader> prescriptionReader,
	std::shared_ptr<BrirReader> brirReader
) :
	compressorFactory{ std::move(compressorFactory) },
	prescriptionReader{ std::move(prescriptionReader) },
	brirReader{ std::move(brirReader) }
{
}

void SpatializedHearingAidSimulationFactory::assertCanBeMade(GlobalTestParameters *p) {
	if (p->usingSpatialization) {
		auto brir = readBrir(p->brirFilePath);
		makeFilter(brir.left);
		makeFilter(brir.right);
	}
	if (p->usingHearingAidSimulation) {
		makeHearingAid(toCompressorParameters(p, {}, readPrescription(p->leftDslPrescriptionFilePath)));
		makeHearingAid(toCompressorParameters(p, {}, readPrescription(p->rightDslPrescriptionFilePath)));
	}
}

std::shared_ptr<AudioFrameProcessor> SpatializedHearingAidSimulationFactory::make(Parameters p) {
	std::vector<std::shared_ptr<SignalProcessor>> processors{};
	const auto brir = global.usingSpatialization 
		? readBrir(global.brirFilePath) 
		: BrirReader::BinauralRoomImpulseResponse{};
	if (p.channelScalars.size() > 0) {
		const auto channel = std::make_shared<SignalProcessingChain>();
		channel->add(std::make_shared<ScalingProcessor>(gsl::narrow_cast<float>(p.channelScalars.at(0))));
		if (global.usingSpatialization)
			channel->add(makeFilter(brir.left));
		if (global.usingHearingAidSimulation)
			channel->add(makeHearingAid(toCompressorParameters(&global, p.sampleRate, readPrescription(global.leftDslPrescriptionFilePath))));
		processors.push_back(channel);
	}
	if (p.channelScalars.size() > 1) {
		const auto channel = std::make_shared<SignalProcessingChain>();
		channel->add(std::make_shared<ScalingProcessor>(gsl::narrow_cast<float>(p.channelScalars.at(1))));
		if (global.usingSpatialization)
			channel->add(makeFilter(brir.right));
		if (global.usingHearingAidSimulation)
			channel->add(makeHearingAid(toCompressorParameters(&global, p.sampleRate, readPrescription(global.rightDslPrescriptionFilePath))));
		processors.push_back(channel);
	}
	return std::make_shared<ChannelProcessingGroup>(processors);
}

BrirReader::BinauralRoomImpulseResponse SpatializedHearingAidSimulationFactory::readBrir(
	std::string filePath
) {
	try {
		return brirReader->read(std::move(filePath));
	}
	catch (const BrirReader::ReadFailure &e) {
		throw CreateError{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeHearingAid(
	FilterbankCompressor::Parameters p
) {
	try {
		return std::make_shared<HearingAidProcessor>(
			compressorFactory->make(std::move(p))
		);
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		throw CreateError{ e.what() };
	}
}

static double max_dB_Spl_Matlab = 119;

int SpatializedHearingAidSimulationFactory::preferredBufferSize() {
	return global.usingHearingAidSimulation ? global.chunkSize : 1024;
}

double SpatializedHearingAidSimulationFactory::fullScale_dB_Spl() {
	return max_dB_Spl_Matlab;
}

PrescriptionReader::Dsl SpatializedHearingAidSimulationFactory::readPrescription(std::string filePath) {
	try {
		return prescriptionReader->read(std::move(filePath));
	}
	catch (const PrescriptionReader::ReadFailure &e) {
		throw CreateError{ e.what() };
	}
}

FilterbankCompressor::Parameters SpatializedHearingAidSimulationFactory::toCompressorParameters(
	GlobalTestParameters *g,
	int sampleRate, 
	PrescriptionReader::Dsl prescription
) {
	FilterbankCompressor::Parameters compression;
	compression.attack_ms = g->attack_ms;
	compression.release_ms = g->release_ms;
	compression.chunkSize = g->chunkSize;
	compression.windowSize = g->windowSize;
	compression.sampleRate = sampleRate;
	compression.max_dB_Spl = max_dB_Spl_Matlab;
	compression.compressionRatios = prescription.compressionRatios;
	compression.crossFrequenciesHz = prescription.crossFrequenciesHz;
	compression.kneepointGains_dB = prescription.kneepointGains_dB;
	compression.kneepoints_dBSpl = prescription.kneepoints_dBSpl;
	compression.broadbandOutputLimitingThresholds_dBSpl = 
		prescription.broadbandOutputLimitingThresholds_dBSpl;
	compression.channels = prescription.channels;
	return compression;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeFilter(std::vector<float> b) {
	try {
		return std::make_shared<FirFilter>(std::move(b));
	}
	catch (const FirFilter::InvalidCoefficients &) {
		throw CreateError{ "Invalid filter coefficients." };
	}
}

void SpatializedHearingAidSimulationFactory::storeParameters(GlobalTestParameters *x) {
	global = *x;
}
