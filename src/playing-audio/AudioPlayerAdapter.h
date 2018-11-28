#pragma once

class SpatializedHearingAidSimulatorFactory {

};

#include "playing-audio-exports.h"
#include <playing-audio/AudioPlayer.h>
#include <audio-device-control/AudioDevice.h>

class AudioPlayerAdapterFactory : public AudioPlayerFactory {
	std::shared_ptr<AudioDeviceFactory> deviceFactory;
public:
	PLAYING_AUDIO_API AudioPlayerAdapterFactory(
		std::shared_ptr<AudioDeviceFactory>,
		std::shared_ptr<SpatializedHearingAidSimulatorFactory>);
	PLAYING_AUDIO_API std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) override;
};
