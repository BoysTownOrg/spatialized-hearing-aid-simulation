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
	device->closeStream();
	device->openStream({});
	device->startStream();
}
