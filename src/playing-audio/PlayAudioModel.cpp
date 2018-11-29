#include "PlayAudioModel.h"
#include <audio-device-control/AudioDeviceController.h>
#include <audio-stream-processing/ProcessedAudioFrameReader.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <hearing-aid-processing/HearingAidProcessor.h>
#include <fir-filtering/FirFilter.h>
#include <signal-processing/ChannelProcessingGroup.h>
#include <signal-processing/SignalProcessingChain.h>
#include <dsl-prescription/DslPrescription.h>

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDeviceFactory> deviceFactory
) :
	deviceFactory{ std::move(deviceFactory) }
{
}

void PlayAudioModel::playRequest(PlayRequest request)
{
	FilterbankCompressor::Parameters forCompressor;
	forCompressor.attack_ms = request.attack_ms;
	forCompressor.release_ms = request.release_ms;
	forCompressor.chunkSize = request.chunkSize;
	forCompressor.windowSize = request.windowSize;
	forCompressor.sampleRate = 44100;
	const auto leftChannel = std::make_shared<SignalProcessingChain>();
	leftChannel->add(std::make_shared<FirFilter>(std::vector<float>{ 1, 2, 3 }));
	leftChannel->add(
		std::make_shared<HearingAidProcessor>(
			compressorFactory->make(
				DslPrescription{ *parserFactory->make(request.leftDslPrescriptionFilePath) },
				forCompressor
			)
		)
	);
	const auto rightChannel = std::make_shared<SignalProcessingChain>();
	rightChannel->add(std::make_shared<FirFilter>(std::vector<float>{ 1, 2, 3 }));
	rightChannel->add(
		std::make_shared<HearingAidProcessor>(
			compressorFactory->make(
				DslPrescription{ *parserFactory->make(request.rightDslPrescriptionFilePath) },
				forCompressor
			)
		)
	);
	AudioDevice::Parameters forDevice;
	forDevice.framesPerBuffer = request.chunkSize;
	forDevice.sampleRate = 44100;
	forDevice.channels = { 0 , 1 };
	const auto reader = audioFileFactory->make(request.audioFilePath);
	AudioDeviceController controller {
		deviceFactory->make(forDevice),
		std::make_shared<ProcessedAudioFrameReader>(
			std::make_shared<AudioFileInMemory>(*reader),
			std::make_shared<ChannelProcessingGroup>(
				std::vector<std::shared_ptr<SignalProcessor>>{ leftChannel, rightChannel }
			)
		)
	};
	controller.startStreaming();
}
