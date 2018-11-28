#pragma once

#include "playing-audio-exports.h"
#include <playing-audio/AudioPlayer.h>

class AudioPlayerAdapterFactory : public AudioPlayerFactory {
public:
	PLAYING_AUDIO_API std::shared_ptr<AudioPlayer> make(AudioPlayer::Parameters) override;
};
