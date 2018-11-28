#pragma once

#include "playing-audio-exports.h"
#include <playing-audio/AudioPlayer.h>
#include <audio-device-control/AudioDevice.h>

class AudioPlayerAdapterFactory : public AudioPlayerFactory {
public:
	PLAYING_AUDIO_API explicit AudioPlayerAdapterFactory(std::shared_ptr<AudioDeviceFactory>);
	PLAYING_AUDIO_API std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) override;
};
