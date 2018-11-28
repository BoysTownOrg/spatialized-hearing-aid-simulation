#include "AudioPlayerAdapter.h"

std::shared_ptr<AudioPlayer> AudioPlayerAdapterFactory::make(AudioPlayer::Parameters) {
	return {};
}
