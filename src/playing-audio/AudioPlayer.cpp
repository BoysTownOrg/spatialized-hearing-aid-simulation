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

void AudioPlayer::play(PlayRequest request) {
	if (device->failed())
		throw DeviceFailure{ device->errorMessage() };
	
	if (device->streaming())
		return;
	
	frameReader = readerFactory->make(request.audioFilePath);
	audio.resize(frameReader->channels());

	AudioFrameProcessorFactory::Parameters forProcessor;
	forProcessor.attack_ms = request.attack_ms;
	forProcessor.release_ms = request.release_ms;
	forProcessor.channels = frameReader->channels();
	forProcessor.brirFilePath = request.brirFilePath;
	forProcessor.leftDslPrescriptionFilePath = request.leftDslPrescriptionFilePath;
	forProcessor.rightDslPrescriptionFilePath = request.rightDslPrescriptionFilePath;
	forProcessor.level_dB_Spl = request.level_dB_Spl;
	forProcessor.sampleRate = frameReader->sampleRate();
	forProcessor.chunkSize = request.chunkSize;
	forProcessor.windowSize = request.windowSize;
	frameProcessor = processorFactory->make(forProcessor);

	AudioDevice::StreamParameters forStreaming;
	forStreaming.sampleRate = frameReader->sampleRate();
	forStreaming.channels = frameReader->channels();
	forStreaming.framesPerBuffer = request.chunkSize;
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			forStreaming.deviceIndex = i;

	device->closeStream();
	device->openStream(forStreaming);
	device->setCallbackResultToContinue();
	device->startStream();
}

std::vector<std::string> AudioPlayer::audioDeviceDescriptions()
{
	return std::vector<std::string>();
}
