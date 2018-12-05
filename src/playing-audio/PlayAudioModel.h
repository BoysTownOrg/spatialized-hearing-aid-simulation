#pragma once

#include "playing-audio-exports.h"
#include "AudioDevice.h"
#include <audio-stream-processing/ProcessedAudioFrameReader.h>
#include <presentation/SpatializedHearingAidSimulationModel.h>
#include <audio-file-reading/AudioFileReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <dsl-prescription/DslPrescription.h>
#include <signal-processing/SignalProcessor.h>
#include <common-includes/RuntimeError.h>

class PlayAudioModel : public SpatializedHearingAidSimulationModel, public AudioDeviceController {
	std::shared_ptr<AudioDevice> device;
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory;
	std::shared_ptr<AudioFileReaderFactory> audioFileFactory;
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory;
	std::shared_ptr<ProcessedAudioFrameReader> streamProcessor{};
public:
	RUNTIME_ERROR(DeviceFailure);
	PLAYING_AUDIO_API PlayAudioModel(
		std::shared_ptr<AudioDevice> device,
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
		std::shared_ptr<AudioFileReaderFactory> audioFileFactory,
		std::shared_ptr<ConfigurationFileParserFactory> parserFactory
	);
	PLAYING_AUDIO_API void play(PlayRequest) override;
	PLAYING_AUDIO_API void fillStreamBuffer(void *channels, int frameCount) override;
	std::vector<std::string> audioDeviceDescriptions() override;
private:
	BinauralRoomImpulseResponse makeBrir(std::string filePath);
	DslPrescription makeDslPrescription(std::string filePath);
	std::shared_ptr<SignalProcessor> makeFilter(std::vector<float> b);
	std::shared_ptr<SignalProcessor> makeHearingAid(
		const DslPrescription &, 
		FilterbankCompressor::Parameters
	);
	std::shared_ptr<AudioFileReader> makeAudioFileReader(std::string filePath);
	std::shared_ptr<AudioFrameReader> makeAudioFrameReader(std::shared_ptr<AudioFileReader>);
};
