#include "PlayAudioModel.h"
#include <audio-device-control/AudioDeviceController.h>
#include <audio-stream-processing/ProcessedAudioFrameReader.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <signal-processing/ScalingProcessor.h>
#include <dsl-prescription/DslPrescription.h>
#include <gsl/gsl>
#include <algorithm>

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDeviceFactory> deviceFactory
) :
	deviceFactory{ std::move(deviceFactory) }
{
}

void PlayAudioModel::playRequest(PlayRequest request) {
	const auto reader = audioFileFactory->make(request.audioFilePath);
	if (reader->failed())
		throw RequestFailure{ reader->errorMessage() };

	std::shared_ptr<AudioFrameReader> frameReader;
	try {
		frameReader = std::make_shared<AudioFileInMemory>(*reader);
	}
	catch (const AudioFileInMemory::FileError &e) {
		throw RequestFailure{ e.what() };
	}

	const auto audioSampleRate = reader->sampleRate();

	FilterbankCompressor::Parameters forCompressor;
	forCompressor.attack_ms = request.attack_ms;
	forCompressor.release_ms = request.release_ms;
	forCompressor.chunkSize = request.chunkSize;
	forCompressor.windowSize = request.windowSize;
	forCompressor.sampleRate = audioSampleRate;

	const auto leftChannel = std::make_shared<SignalProcessingChain>();
	leftChannel->add(std::make_shared<ScalingProcessor>(0.5f));

	const auto brirParser = parserFactory->make(request.brirFilePath);

	int brirSampleRate;
	try {
		brirSampleRate = brirParser->asInt("sample rate");
	}
	catch (const ConfigurationFileParser::ParseError &e) {
		throw RequestFailure{ e.what() };
	}

	if (brirSampleRate != audioSampleRate)
		throw RequestFailure{ "Not sure what to do with different sample rates." };

	std::vector<double> leftImpulseResponse;
	try {
		leftImpulseResponse = brirParser->asVector("left impulse response");
	}
	catch (const ConfigurationFileParser::ParseError &e) {
		throw RequestFailure{ e.what() };
	}

	std::vector<float> leftImpulseAsFloat;
	std::transform(
		leftImpulseResponse.begin(), 
		leftImpulseResponse.end(), 
		std::back_inserter(leftImpulseAsFloat),
		[](double x) -> float { return gsl::narrow_cast<float>(x); });

	std::shared_ptr<SignalProcessor> leftFilter;
	try {
		leftFilter = std::make_shared<FirFilter>(leftImpulseAsFloat);
	}
	catch (const FirFilter::InvalidCoefficients &) {
		throw RequestFailure{ "bad coefficients?" };
	}
	leftChannel->add(leftFilter);

	std::shared_ptr<DslPrescription> leftPrescription;
	try {
		leftPrescription = std::make_shared<DslPrescription>(
			*parserFactory->make(request.leftDslPrescriptionFilePath));
	}
	catch (const DslPrescription::InvalidPrescription &e) {
		throw RequestFailure{ e.what() };
	}

	std::shared_ptr<SignalProcessor> leftHearingAid;
	try {
		leftHearingAid = std::make_shared<HearingAidProcessor>(
			compressorFactory->make(*leftPrescription, forCompressor)
		);
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		throw RequestFailure{ e.what() };
	}
	leftChannel->add(leftHearingAid);

	const auto rightChannel = std::make_shared<SignalProcessingChain>();
	rightChannel->add(std::make_shared<ScalingProcessor>(0.5f));

	std::vector<double> rightImpulseResponse;
	try {
		rightImpulseResponse = brirParser->asVector("right impulse response");
	}
	catch (const ConfigurationFileParser::ParseError &e) {
		throw RequestFailure{ e.what() };
	}

	std::vector<float> rightImpulseAsFloat;
	std::transform(
		rightImpulseResponse.begin(),
		rightImpulseResponse.end(),
		std::back_inserter(rightImpulseAsFloat),
		[](double x) -> float { return gsl::narrow_cast<float>(x); });

	std::shared_ptr<SignalProcessor> rightFilter;
	try {
		rightFilter = std::make_shared<FirFilter>(rightImpulseAsFloat);
	}
	catch (const FirFilter::InvalidCoefficients &) {
		throw RequestFailure{ "" };
	}
	rightChannel->add(rightFilter);

	std::shared_ptr<DslPrescription> rightPrescription;
	try {
		rightPrescription = std::make_shared<DslPrescription>(
			*parserFactory->make(request.rightDslPrescriptionFilePath));
	}
	catch (const DslPrescription::InvalidPrescription &e) {
		throw RequestFailure{ e.what() };
	}

	std::shared_ptr<SignalProcessor> rightHearingAid;
	try {
		rightHearingAid = std::make_shared<HearingAidProcessor>(
			compressorFactory->make(*rightPrescription, forCompressor)
		);
	}
	catch (const HearingAidProcessor::CompressorError &e) {
		throw RequestFailure{ e.what() };
	}
	rightChannel->add(rightHearingAid);

	AudioDevice::Parameters forDevice;
	forDevice.framesPerBuffer = request.chunkSize;
	forDevice.sampleRate = audioSampleRate;
	forDevice.channels = { 0, 1 };

	try {
		AudioDeviceController controller{
			deviceFactory->make(forDevice),
			std::make_shared<ProcessedAudioFrameReader>(
				frameReader,
				std::make_shared<ChannelProcessingGroup>(
					std::vector<std::shared_ptr<SignalProcessor>>{ leftChannel, rightChannel }
				)
			)
		};
		controller.startStreaming();
	}
	catch (const AudioDeviceController::DeviceConnectionFailure &e) {
		throw RequestFailure{ e.what() };
	}
	catch (const AudioDeviceController::StreamingError &e) {
		throw RequestFailure{ e.what() };
	}
}
