#include "PlayAudioModel.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ScalingProcessor.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDevice> device,
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
	std::shared_ptr<AudioFileReaderFactory> audioFileFactory,
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory
) :
	device{ std::move(device) },
	compressorFactory{ std::move(compressorFactory) },
	audioFileFactory{ std::move(audioFileFactory) },
	parserFactory{ std::move(parserFactory) }
{
	if (this->device->failed())
		throw DeviceFailure{ this->device->errorMessage() };
	this->device->setController(this);
}

void PlayAudioModel::play(PlayRequest request) {
	if (device->streaming())
		return;

	const auto reader = makeAudioFileReader(request.audioFilePath);
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

	const auto leftChannel = makeChannel(brir.left(), request.leftDslPrescriptionFilePath, forCompressor);
	const auto rightChannel = makeChannel(brir.right(), request.rightDslPrescriptionFilePath, forCompressor);

	AudioDevice::StreamParameters forStreaming;
	forStreaming.framesPerBuffer = request.chunkSize;
	forStreaming.sampleRate = reader->sampleRate();
	forStreaming.channels = { 0, 1 };

	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			forStreaming.deviceIndex = i;

	streamProcessor = std::make_shared<ProcessedAudioFrameReader>(
		makeAudioFrameReader(reader),
		std::make_shared<ChannelProcessingGroup>(
			std::vector<std::shared_ptr<SignalProcessor>>{ leftChannel, rightChannel }
		)
	);
	device->closeStream();
	device->openStream(forStreaming);
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
	device->setCallbackResultToContinue();
	device->startStream();
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
}

void PlayAudioModel::fillStreamBuffer(void * channels, int frameCount) {
	streamProcessor->read(static_cast<float **>(channels), frameCount);
	if (streamProcessor->complete())
		device->setCallbackResultToComplete();
}

BinauralRoomImpulseResponse PlayAudioModel::makeBrir(std::string filePath) {
	try {
		return BinauralRoomImpulseResponse{ *parserFactory->make(filePath) };
	}
	catch (const BinauralRoomImpulseResponse::InvalidResponse &e) {
		throw RequestFailure{ e.what() };
	}
}

std::shared_ptr<SignalProcessor> PlayAudioModel::makeChannel(
	std::vector<float> b, 
	std::string filePath,
	FilterbankCompressor::Parameters forCompressor
) {
	const auto channel = std::make_shared<SignalProcessingChain>();
	channel->add(std::make_shared<ScalingProcessor>(0.5f));
	channel->add(makeFilter(b));
	channel->add(
		makeHearingAid(
			makeDslPrescription(filePath),
			forCompressor));
	return channel;
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

std::shared_ptr<AudioFrameReader> PlayAudioModel::makeAudioFrameReader(
	std::shared_ptr<AudioFileReader> reader
) {
	const auto inMemory = std::make_shared<AudioFileInMemory>(*reader);
	if (reader->channels() == 1)
		return std::make_shared<ChannelCopier>(inMemory);
	else
		return inMemory;
}

std::vector<std::string> PlayAudioModel::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}
