#include "AudioPlayerAdapter.h"

AudioPlayerAdapterFactory::AudioPlayerAdapterFactory(std::shared_ptr<AudioDeviceFactory>)
{
}

std::shared_ptr<AudioPlayer> AudioPlayerAdapterFactory::make(AudioPlayer::Parameters) {
	return {};
}
