#include "PlayAudioModel.h"

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDeviceFactory> deviceFactory
) :
	deviceFactory{ std::move(deviceFactory) }
{
}

void PlayAudioModel::playRequest(PlayRequest request)
{
	AudioDevice::Parameters forDevice;
	forDevice.framesPerBuffer = request.chunkSize;
	deviceFactory->make(forDevice);
}
