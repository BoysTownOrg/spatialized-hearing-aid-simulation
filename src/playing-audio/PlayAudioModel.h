#pragma once

#include "playing-audio-exports.h"
#include <presentation/SpatializedHearingAidSimulationModel.h>
#include <audio-file-reading/AudioFileReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <audio-device-control/AudioDevice.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <binaural-room-impulse-response-config/BinauralRoomImpulseResponse.h>
#include <dsl-prescription/DslPrescription.h>

class PlayAudioModel : public SpatializedHearingAidSimulationModel {
	std::shared_ptr<AudioDeviceFactory> deviceFactory;
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory;
	std::shared_ptr<AudioFileReaderFactory> audioFileFactory;
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory;
public:
	PLAYING_AUDIO_API PlayAudioModel(
		std::shared_ptr<AudioDeviceFactory> deviceFactory,
		std::shared_ptr<FilterbankCompressorFactory> compressorFactory,
		std::shared_ptr<AudioFileReaderFactory> audioFileFactory,
		std::shared_ptr<ConfigurationFileParserFactory> parserFactory
	);
	PLAYING_AUDIO_API void playRequest(PlayRequest) override;
private:
	BinauralRoomImpulseResponse makeBrir(std::string filePath);
	DslPrescription makeDslPrescription(std::string filePath);
};
