#pragma once

#include <common-includes/Interface.h>

class SpatializedHearingAidSimulator {
public:
	struct Parameters {
		double attack_ms;
	};
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulator);
};

#include <memory>

class SpatializedHearingAidSimulatorFactory {
public:
	INTERFACE_OPERATIONS(SpatializedHearingAidSimulatorFactory);
	virtual std::shared_ptr<SpatializedHearingAidSimulator> make(
		SpatializedHearingAidSimulator::Parameters) = 0;
};

#include "playing-audio-exports.h"
#include <playing-audio/AudioPlayer.h>
#include <audio-device-control/AudioDevice.h>

class AudioPlayerAdapterFactory : public AudioPlayerFactory {
	std::shared_ptr<AudioDeviceFactory> deviceFactory;
	std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory;
public:
	PLAYING_AUDIO_API AudioPlayerAdapterFactory(
		std::shared_ptr<AudioDeviceFactory>,
		std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory);
	PLAYING_AUDIO_API std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) override;
};
