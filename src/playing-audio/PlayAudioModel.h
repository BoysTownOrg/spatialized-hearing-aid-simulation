#pragma once

#include "playing-audio-exports.h"
#include <presentation/SpatializedHearingAidSimulationModel.h>
#include <audio-file-reading/AudioFileReader.h>
#include <hearing-aid-processing/FilterbankCompressor.h>
#include <audio-device-control/AudioDevice.h>
#include <dsl-prescription/ConfigurationFileParser.h>

class PlayAudioModel : public SpatializedHearingAidSimulationModel {
	std::shared_ptr<AudioDeviceFactory> deviceFactory;
	std::shared_ptr<FilterbankCompressorFactory> compressorFactory;
	std::shared_ptr<AudioFileReaderFactory> audioFileFactory;
	std::shared_ptr<ConfigurationFileParserFactory> parserFactory;
public:
	PLAYING_AUDIO_API PlayAudioModel(
		std::shared_ptr<AudioDeviceFactory>
	);
	PLAYING_AUDIO_API void playRequest(PlayRequest) override;
};
