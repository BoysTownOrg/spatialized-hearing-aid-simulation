#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(
	AudioDevice *device, 
	AudioFrameReaderFactory *readerFactory, 
	AudioFrameProcessorFactory *processorFactory
) :
	device{ device },
	readerFactory{ readerFactory },
	processorFactory{ processorFactory }
{
	device->setController(this);
}

void AudioPlayer::fillStreamBuffer(void * channels, int frames) {
	channels;
	frames;
	if (frameReader->complete())
		device->setCallbackResultToComplete();
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio[i] = { static_cast<float **>(channels)[i], frames };
	frameProcessor->process(audio);
	frameReader->read(audio);
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
	audio.resize(frameReader->channels());
	frameProcessor = processorFactory->make({});
}
