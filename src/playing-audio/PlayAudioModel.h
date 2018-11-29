#pragma once

#include "playing-audio-exports.h"
#include <presentation/SpatializedHearingAidSimulationModel.h>
#include <audio-device-control/AudioDevice.h>

class PlayAudioModel : public SpatializedHearingAidSimulationModel {
	std::shared_ptr<AudioDeviceFactory> deviceFactory;
public:
	PLAYING_AUDIO_API PlayAudioModel(
		std::shared_ptr<AudioDeviceFactory>
	);
	PLAYING_AUDIO_API void playRequest(PlayRequest) override;
};
