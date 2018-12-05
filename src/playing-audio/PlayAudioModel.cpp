#include "PlayAudioModel.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDevice> device,
	std::shared_ptr<AudioFrameReaderFactory> readerFactory,
	std::shared_ptr<AudioFrameProcessorFactory> processorFactory
) :
	device{ std::move(device) },
	readerFactory{ std::move(readerFactory) },
	processorFactory{ std::move(processorFactory) }
{
	if (this->device->failed())
		throw DeviceFailure{ this->device->errorMessage() };
	this->device->setController(this);
}

void PlayAudioModel::play(PlayRequest request) {
	if (device->streaming())
		return;

	try {
		frameReader = readerFactory->make(request.audioFilePath);
	}
	catch (const AudioFrameReaderFactory::FileError &e) {
		throw RequestFailure{ e.what() };
	}

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

	try {
		frameProcessor = processorFactory->make(forProcessor);
	}
	catch (const AudioFrameProcessorFactory::Failure &e) {
		throw RequestFailure{ e.what() };
	}

	AudioDevice::StreamParameters forStreaming;
	forStreaming.framesPerBuffer = request.chunkSize;
	forStreaming.sampleRate = frameReader->sampleRate();
	forStreaming.channels = frameReader->channels();

	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			forStreaming.deviceIndex = i;
	device->closeStream();
	device->openStream(forStreaming);
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
	device->setCallbackResultToContinue();
	device->startStream();
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
}

void PlayAudioModel::fillStreamBuffer(void * channels, int frames) {
	const auto audio = static_cast<float **>(channels);
	frameReader->read(audio, frames);
	frameProcessor->process(audio, frames);
	if (frameReader->complete())
		device->setCallbackResultToComplete();
}

std::vector<std::string> PlayAudioModel::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}
