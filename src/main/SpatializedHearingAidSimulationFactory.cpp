#include "SpatializedHearingAidSimulationFactory.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ScalingProcessor.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <presentation/Presenter.h>
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
	const auto brir = readBrir(p.global->brirFilePath);
	processors.push_back(makeChannel(
		brir.left,
		toCompressorParameters(p, readPrescription(p.global->leftDslPrescriptionFilePath)),
		p.channelScalars.at(0))
	);
	processors.push_back(makeChannel(
		brir.right,
		toCompressorParameters(p, readPrescription(p.global->rightDslPrescriptionFilePath)),
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

int SpatializedHearingAidSimulationFactory::preferredBufferSize() {
	return 0;
}

double SpatializedHearingAidSimulationFactory::fullScale_dB_Spl() {
	return 0.0;
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
	Parameters p, 
	PrescriptionReader::Dsl prescription
) {
	FilterbankCompressor::Parameters compression;
	compression.attack_ms = p.global->attack_ms;
	compression.release_ms = p.global->release_ms;
	compression.chunkSize = p.global->chunkSize;
	compression.windowSize = p.global->windowSize;
	compression.sampleRate = p.sampleRate;
	compression.max_dB_Spl = p.global->max_dB_Spl;
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