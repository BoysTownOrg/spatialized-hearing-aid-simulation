#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(AudioDevice * device) :
	device{ device }
{
	device->setController(this);
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	channels;
	frames;
}

void AudioPlayer::play(std::string filePath)
{
	filePath;
	if (device->failed())
		throw DeviceFailure{ device->errorMessage() };
	device->closeStream();
	device->openStream({});
	device->startStream();
}
