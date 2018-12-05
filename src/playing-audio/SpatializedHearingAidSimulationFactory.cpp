#include "SpatializedHearingAidSimulationFactory.h"
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ScalingProcessor.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>

SpatializedHearingAidSimulationFactory::SpatializedHearingAidSimulationFactory(
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory
) :
	compressorFactory{ std::move(compressorFactory) },
	parserFactory{ std::move(parserFactory) }
{
}

std::shared_ptr<AudioFrameProcessor> SpatializedHearingAidSimulationFactory::make(Parameters p)
{
	if (p.channels != 2)
		throw Failure{ "Can't process other than two channels." };

	FilterbankCompressor::Parameters forCompressor;
	forCompressor.attack_ms = p.attack_ms;
	forCompressor.release_ms = p.release_ms;
	forCompressor.chunkSize = p.chunkSize;
	forCompressor.windowSize = p.windowSize;
	forCompressor.sampleRate = p.sampleRate;
	forCompressor.max_dB = 119;

	const auto brir = makeBrir(p.brirFilePath);
	if (brir.sampleRate() != p.sampleRate)
		throw Failure{ "Not sure what to do with different sample rates." };

	return std::make_shared<ChannelProcessingGroup>(
		std::vector<std::shared_ptr<SignalProcessor>>{
			makeChannel(brir.left(), p.leftDslPrescriptionFilePath, forCompressor),
			makeChannel(brir.right(), p.rightDslPrescriptionFilePath, forCompressor)
		}
	);
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeChannel(
	std::vector<float> b,
	std::string prescriptionFilePath,
	FilterbankCompressor::Parameters forCompressor
) {
	const auto channel = std::make_shared<SignalProcessingChain>();
	channel->add(std::make_shared<ScalingProcessor>(0.5f));
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
		throw Failure{ e.what() };
	}
}

DslPrescription SpatializedHearingAidSimulationFactory::makeDslPrescription(std::string filePath) {
	try {
		return DslPrescription{ *parserFactory->make(filePath) };
	}
	catch (const DslPrescription::InvalidPrescription &e) {
		throw Failure{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> SpatializedHearingAidSimulationFactory::makeFilter(std::vector<float> b) {
	try {
		return std::make_shared<FirFilter>(b);
	}
	catch (const FirFilter::InvalidCoefficients &) {
		throw Failure{ "The impulse response is empty?" };
	}
}

BinauralRoomImpulseResponse SpatializedHearingAidSimulationFactory::makeBrir(std::string filePath) {
	try {
		return BinauralRoomImpulseResponse{ *parserFactory->make(filePath) };
	}
	catch (const BinauralRoomImpulseResponse::InvalidResponse &e) {
		throw Failure{ e.what() };
	}
}