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
	if (p.channels) {
		const auto brir = readBrir(p.brirFilePath);
		if (brir.sampleRate != p.sampleRate)
			throw CreateError{ "Not sure what to do with different sample rates." };
		processors.push_back(makeChannel(
			brir.left,
			toCompressorParameters(p, readPrescription(p.leftDslPrescriptionFilePath)),
			p.stimulusRms.at(0),
			p.level_dB_Spl)
		);
		if (p.channels > 1)
			processors.push_back(makeChannel(
				brir.right,
				toCompressorParameters(p, readPrescription(p.rightDslPrescriptionFilePath)),
				p.stimulusRms.at(1),
				p.level_dB_Spl)
			);
	}

	return std::make_shared<ChannelProcessingGroup>(processors);
}

BrirReader::BinauralRoomImpulseResponse SpatializedHearingAidSimulationFactory::readBrir(
	std::string filePath
) {
	try {
		return brirReader->read(filePath);
	}
	catch (const BrirReader::ReadError &e) {
		throw CreateError{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeChannel(
	std::vector<float> b,
	FilterbankCompressor::Parameters compression,
	double rms,
	double level_dB_Spl
) {
	const auto channel = std::make_shared<SignalProcessingChain>();
	const auto scale = std::pow(10.0, (level_dB_Spl - compression.max_dB) / 20.0) / rms;
	channel->add(std::make_shared<ScalingProcessor>(gsl::narrow_cast<float>(scale)));
	channel->add(makeFilter(b));
	channel->add(makeHearingAid(compression));
	return channel;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeHearingAid(
	FilterbankCompressor::Parameters p
) {
	try {
		return std::make_shared<HearingAidProcessor>(
			compressorFactory->make(p)
		);
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		throw CreateError{ e.what() };
	}
}

PrescriptionReader::Dsl SpatializedHearingAidSimulationFactory::readPrescription(std::string filePath) {
	try {
		return prescriptionReader->read(filePath);
	}
	catch (const PrescriptionReader::ReadError &e) {
		throw CreateError{ e.what() };
	}
}

FilterbankCompressor::Parameters SpatializedHearingAidSimulationFactory::toCompressorParameters(
	Parameters p, 
	PrescriptionReader::Dsl prescription
) {
	FilterbankCompressor::Parameters compression;
	compression.attack_ms = p.attack_ms;
	compression.release_ms = p.release_ms;
	compression.chunkSize = p.chunkSize;
	compression.windowSize = p.windowSize;
	compression.sampleRate = p.sampleRate;
	compression.max_dB = 119;
	compression.compressionRatios = prescription.compressionRatios;
	compression.crossFrequenciesHz = prescription.crossFrequenciesHz;
	compression.kneepointGains_dB = prescription.kneepointGains_dB;
	compression.kneepoints_dBSpl = prescription.kneepoints_dBSpl;
	compression.broadbandOutputLimitingThresholds_dBSpl = prescription.broadbandOutputLimitingThresholds_dBSpl;
	compression.channels = prescription.channels;
	return compression;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeFilter(std::vector<float> b) {
	try {
		return std::make_shared<FirFilter>(b);
	}
	catch (const FirFilter::InvalidCoefficients &) {
		throw CreateError{ "Invalid filter coefficients." };
	}
}