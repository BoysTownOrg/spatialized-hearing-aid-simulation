#include "AudioPlayerAdapter.h"

AudioPlayerAdapterFactory::AudioPlayerAdapterFactory(
	std::shared_ptr<AudioDeviceFactory> deviceFactory,
	std::shared_ptr<SpatializedHearingAidSimulatorFactory>
) :
	deviceFactory{ std::move(deviceFactory) }
{
}

std::shared_ptr<AudioPlayer> AudioPlayerAdapterFactory::make(AudioPlayer::Parameters p) {
	AudioDevice::Parameters forDevice{};
	forDevice.framesPerBuffer = p.forAudioDevice.framesPerBuffer;
	forDevice.sampleRate = p.forAudioDevice.sampleRate;
	deviceFactory->make(forDevice);
	return {};
}
