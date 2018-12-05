#include "PlayAudioModel.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>

PlayAudioModel::PlayAudioModel(
	std::shared_ptr<AudioDevice> device,
	std::shared_ptr<AudioFileReaderFactory> audioFileFactory,
	std::shared_ptr<AudioFrameProcessorFactory> processorFactory
) :
	device{ std::move(device) },
	audioFileFactory{ std::move(audioFileFactory) },
	processorFactory{ std::move(processorFactory) }
{
	if (this->device->failed())
		throw DeviceFailure{ this->device->errorMessage() };
	this->device->setController(this);
}

void PlayAudioModel::play(PlayRequest request) {
	if (device->streaming())
		return;

	frameReader = makeAudioFrameReader(request.audioFilePath);
	auto channels = frameReader->channels();
	if (channels == 1) {
		frameReader = std::make_shared<ChannelCopier>(frameReader);
		channels = 2;
	}
	AudioFrameProcessorFactory::Parameters forProcessor;
	forProcessor.attack_ms = request.attack_ms;
	forProcessor.release_ms = request.release_ms;
	forProcessor.channels = channels;
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
	forStreaming.channels = channels;

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

std::shared_ptr<AudioFrameReader> PlayAudioModel::makeAudioFrameReader(std::string filePath) {
	return std::make_shared<AudioFileInMemory>(*makeAudioFileReader(filePath));
}

std::shared_ptr<AudioFileReader> PlayAudioModel::makeAudioFileReader(std::string filePath) {
	const auto reader = audioFileFactory->make(filePath);
	if (reader->failed())
		throw RequestFailure{ reader->errorMessage() };
	return reader;
}

std::vector<std::string> PlayAudioModel::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}
