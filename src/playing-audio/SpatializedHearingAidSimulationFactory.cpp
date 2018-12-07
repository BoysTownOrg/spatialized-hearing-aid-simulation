#include "SpatializedHearingAidSimulationFactory.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ScalingProcessor.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <gsl/gsl>

SpatializedHearingAidSimulationFactory::SpatializedHearingAidSimulationFactory(
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory,
	std::shared_ptr<BrirReader> brirReader
) :
	compressorFactory{ std::move(compressorFactory) },
	parserFactory{ std::move(parserFactory) },
	brirReader{ std::move(brirReader) }
{
}

std::shared_ptr<AudioFrameProcessor> SpatializedHearingAidSimulationFactory::make(Parameters p) {
	FilterbankCompressor::Parameters forCompressor;
	forCompressor.attack_ms = p.attack_ms;
	forCompressor.release_ms = p.release_ms;
	forCompressor.chunkSize = p.chunkSize;
	forCompressor.windowSize = p.windowSize;
	forCompressor.sampleRate = p.sampleRate;
	forCompressor.max_dB = 119;

	const auto brir = readBrir(p.brirFilePath);
	if (brir.sampleRate != p.sampleRate)
		throw CreateError{ "Not sure what to do with different sample rates." };

	std::vector<std::shared_ptr<SignalProcessor>> processors{};
	if (p.channels > 0)
		processors.push_back(makeChannel(brir.left, p.leftDslPrescriptionFilePath, forCompressor, p.stimulusRms[0], p.level_dB_Spl));
	if (p.channels > 1)
		processors.push_back(makeChannel(brir.right, p.rightDslPrescriptionFilePath, forCompressor, p.stimulusRms[1], p.level_dB_Spl));

	return std::make_shared<ChannelProcessingGroup>(processors);
}

BrirReader::BinauralRoomImpulseResponse SpatializedHearingAidSimulationFactory::readBrir(
	std::string filePath)
{
	try {
		return brirReader->read(filePath);
	}
	catch (const BrirReader::ReadError &e) {
		throw CreateError{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeChannel(
	std::vector<float> b,
	std::string prescriptionFilePath,
	FilterbankCompressor::Parameters forCompressor,
	double rms,
	double level_dB_Spl
) {
	const auto channel = std::make_shared<SignalProcessingChain>();
	const auto scale = std::pow(10.0, (level_dB_Spl - forCompressor.max_dB) / 20.0) / rms;
	channel->add(std::make_shared<ScalingProcessor>(gsl::narrow_cast<float>(scale)));
	channel->add(makeFilter(b));
	channel->add(
		makeHearingAid(
			makeDslPrescription(prescriptionFilePath),
			forCompressor));
	return channel;
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeHearingAid(
	const DslPrescription &prescription,
	FilterbankCompressor::Parameters parameters)
{
	try {
		return std::make_shared<HearingAidProcessor>(
			compressorFactory->make(prescription, parameters)
		);
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		throw CreateError{ e.what() };
	}
}

DslPrescription SpatializedHearingAidSimulationFactory::makeDslPrescription(std::string filePath) {
	try {
		return DslPrescription{ *parserFactory->make(filePath) };
	}
	catch (const DslPrescription::InvalidPrescription &e) {
		throw CreateError{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeFilter(std::vector<float> b) {
	try {
		return std::make_shared<FirFilter>(b);
	}
	catch (const FirFilter::InvalidCoefficients &) {
		throw CreateError{ "The impulse response is empty?" };
	}
}