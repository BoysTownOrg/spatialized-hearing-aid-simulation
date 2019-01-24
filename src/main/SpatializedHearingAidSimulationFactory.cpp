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

std::shared_ptr<AudioFrameProcessor> SpatializedHearingAidSimulationFactory::make(Parameters p) {
	std::vector<std::shared_ptr<SignalProcessor>> processors{};
	const auto brir = readBrir(global.brirFilePath);
	processors.push_back(makeChannel(
		brir.left,
		toCompressorParameters(&global, p, readPrescription(global.leftDslPrescriptionFilePath)),
		p.channelScalars.at(0))
	);
	processors.push_back(makeChannel(
		brir.right,
		toCompressorParameters(&global, p, readPrescription(global.rightDslPrescriptionFilePath)),
		p.channelScalars.at(1))
	);
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

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeChannel(
	std::vector<float> b,
	FilterbankCompressor::Parameters compression,
	double scale
) {
	const auto channel = std::make_shared<SignalProcessingChain>();
	channel->add(std::make_shared<ScalingProcessor>(gsl::narrow_cast<float>(scale)));
	channel->add(makeFilter(std::move(b)));
	channel->add(makeHearingAid(std::move(compression)));
	return channel;
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

void SpatializedHearingAidSimulationFactory::assertCanBeMade(GlobalTestParameters *x) {
	const auto brir = readBrir(x->brirFilePath);
	Parameters dummy;
	makeChannel(
		brir.left,
		toCompressorParameters(x, dummy, readPrescription(x->leftDslPrescriptionFilePath)),
		0
	);
	makeChannel(
		brir.right,
		toCompressorParameters(x, dummy, readPrescription(x->rightDslPrescriptionFilePath)),
		0
	);
}

static double max_dB_Spl_Matlab = 119;

void SpatializedHearingAidSimulationFactory::storeParameters(GlobalTestParameters *x) {
	global = *x;
}

int SpatializedHearingAidSimulationFactory::preferredBufferSize() {
	return global.chunkSize;
}

double SpatializedHearingAidSimulationFactory::fullScale_dB_Spl() {
	return max_dB_Spl_Matlab;
}

std::vector<int> SpatializedHearingAidSimulationFactory::preferredProcessingSizes() {
	return { 64, 128, 256, 512, 1024, 2048, 4096, 8192 };
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
	Parameters p, 
	PrescriptionReader::Dsl prescription
) {
	FilterbankCompressor::Parameters compression;
	compression.attack_ms = g->attack_ms;
	compression.release_ms = g->release_ms;
	compression.chunkSize = g->chunkSize;
	compression.windowSize = g->windowSize;
	compression.sampleRate = p.sampleRate;
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