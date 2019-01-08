#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(AudioDevice *device, AudioFrameReaderFactory *readerFactory) :
	device{ device },
	readerFactory{ readerFactory }
{
	device->setController(this);
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	channels;
	frames;
	if (frameReader->complete())
		device->setCallbackResultToComplete();
}

void AudioPlayer::play(std::string filePath)
{
	filePath;
	if (device->failed())
		throw DeviceFailure{ device->errorMessage() };
	if (device->streaming())
		return;
	device->closeStream();
	device->openStream({});
	device->startStream();
	frameReader = readerFactory->make({});
}
