#include "AudioPlayerAdapter.h"

AudioPlayerAdapterFactory::AudioPlayerAdapterFactory(
	std::shared_ptr<AudioDeviceFactory> deviceFactory,
	std::shared_ptr<SpatializedHearingAidSimulatorFactory> simulatorFactory
) :
	deviceFactory{ std::move(deviceFactory) },
	simulatorFactory{ std::move(simulatorFactory) }
{
}

std::shared_ptr<AudioPlayer> AudioPlayerAdapterFactory::make(AudioPlayer::Parameters p) {
	AudioDevice::Parameters forDevice{};
	forDevice.framesPerBuffer = p.forAudioDevice.framesPerBuffer;
	forDevice.sampleRate = p.forAudioDevice.sampleRate;
	deviceFactory->make(forDevice);
	SpatializedHearingAidSimulator::Parameters forSimulator{};
	forSimulator.attack_ms = p.forHearingAidSimulation.attack_ms;
	simulatorFactory->make(forSimulator);
	return {};
}
