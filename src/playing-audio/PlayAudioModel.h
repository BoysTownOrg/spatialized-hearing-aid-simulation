#pragma once

#include "playing-audio-exports.h"
#include "SpatializedHearingAidSimulator.h"
#include <presentation/SpatializedHearingAidSimulationModel.h>
#include <audio-device-control/AudioDevice.h>

class PlayAudioModel : public SpatializedHearingAidSimulationModel {
	std::shared_ptr<AudioDeviceFactory> deviceFactory;
	std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory;
public:
	PLAYING_AUDIO_API PlayAudioModel(
		std::shared_ptr<AudioDeviceFactory>,
		std::shared_ptr<SpatializedHearingAidSimulatorFactory>
	);
	PLAYING_AUDIO_API void playRequest(PlayRequest) override;
private:
	void throwIfNotDouble(std::string x, std::string identifier);
	void throwIfNotPositiveInteger(std::string x, std::string identifier);
	void throwRequestFailure(std::string x, std::string identifier);
};
