#include "PlayAudioModel.h"
#include <audio-stream-processing/ProcessedAudioFrameReader.h>
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ScalingProcessor.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDeviceFactory> deviceFactory,
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
	std::shared_ptr<AudioFileReaderFactory> audioFileFactory,
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory
) :
	deviceFactory{ std::move(deviceFactory) },
	compressorFactory{ std::move(compressorFactory) },
	audioFileFactory{ std::move(audioFileFactory) },
	parserFactory{ std::move(parserFactory) }
{
}

void PlayAudioModel::playRequest(PlayRequest request) {
	if (controller->active())
		return;
	const auto reader = makeAudioFileReader(request.audioFilePath);
	
	std::shared_ptr<AudioFrameReader> frameReader = std::make_shared<AudioFileInMemory>(*reader);
	if (reader->channels() == 1)
		frameReader = std::make_shared<ChannelCopier>(frameReader);

	const auto brir = makeBrir(request.brirFilePath);

	if (brir.sampleRate() != reader->sampleRate())
		throw RequestFailure{ "Not sure what to do with different sample rates." };

	FilterbankCompressor::Parameters forCompressor;
	forCompressor.attack_ms = request.attack_ms;
	forCompressor.release_ms = request.release_ms;
	forCompressor.chunkSize = request.chunkSize;
	forCompressor.windowSize = request.windowSize;
	forCompressor.sampleRate = reader->sampleRate();
	forCompressor.max_dB = 119;

	const auto leftChannel = std::make_shared<SignalProcessingChain>();
	
	leftChannel->add(std::make_shared<ScalingProcessor>(0.5f));

	leftChannel->add(makeFilter(brir.left()));

	leftChannel->add(
		makeHearingAid(
			makeDslPrescription(request.leftDslPrescriptionFilePath),
			forCompressor));

	const auto rightChannel = std::make_shared<SignalProcessingChain>();
	
	rightChannel->add(std::make_shared<ScalingProcessor>(0.5f));
	
	rightChannel->add(makeFilter(brir.right()));

	rightChannel->add(
		makeHearingAid(
			makeDslPrescription(request.rightDslPrescriptionFilePath),
			forCompressor));

	AudioDevice::Parameters forDevice;
	forDevice.framesPerBuffer = request.chunkSize;
	forDevice.sampleRate = reader->sampleRate();
	forDevice.channels = { 0, 1 };

	try {
		controller = std::make_unique<AudioDeviceController>(
			deviceFactory->make(forDevice),
			std::make_shared<ProcessedAudioFrameReader>(
				frameReader,
				std::make_shared<ChannelProcessingGroup>(
					std::vector<std::shared_ptr<SignalProcessor>>{ leftChannel, rightChannel }
				)
			)
		);
		controller->startStreaming();
	}
	catch (const AudioDeviceController::DeviceConnectionFailure &e) {
		throw RequestFailure{ e.what() };
	}
	catch (const AudioDeviceController::StreamingError &e) {
		throw RequestFailure{ e.what() };
	}
}

BinauralRoomImpulseResponse PlayAudioModel::makeBrir(std::string filePath) {
	try {
		return BinauralRoomImpulseResponse{ *parserFactory->make(filePath) };
	}
	catch (const BinauralRoomImpulseResponse::InvalidResponse &e) {
		throw RequestFailure{ e.what() };
	}
}

DslPrescription PlayAudioModel::makeDslPrescription(std::string filePath) {
	try {
		return DslPrescription{ *parserFactory->make(filePath) };
	}
	catch (const DslPrescription::InvalidPrescription &e) {
		throw RequestFailure{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> PlayAudioModel::makeFilter(std::vector<float> b) {
	try {
		return std::make_shared<FirFilter>(b);
	}
	catch (const FirFilter::InvalidCoefficients &) {
		throw RequestFailure{ "The impulse response is empty?" };
	}
}

std::shared_ptr<SignalProcessor> PlayAudioModel::makeHearingAid(
	const DslPrescription &prescription, 
	FilterbankCompressor::Parameters parameters)
{
	try {
		return std::make_shared<HearingAidProcessor>(
			compressorFactory->make(prescription, parameters)
		);
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		throw RequestFailure{ e.what() };
	}
}

std::shared_ptr<AudioFileReader> PlayAudioModel::makeAudioFileReader(std::string filePath) {
	const auto reader = audioFileFactory->make(filePath);
	if (reader->failed())
		throw RequestFailure{ reader->errorMessage() };
	return reader;
}
