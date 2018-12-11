#include "PlayAudioModel.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <gsl/gsl>

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

	frameReader = makeReader(request.audioFilePath);

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

	std::vector<std::vector<float>> entireAudioFile(frameReader->channels());
	std::vector<gsl::span<float>> pointers;
	for (auto &channel : entireAudioFile)
		channel.resize(gsl::narrow<std::vector<float>::size_type>(frameReader->frames()));
	for (auto &channel : entireAudioFile)
		if (channel.size() > 0)
			pointers.push_back({ &channel[0], gsl::narrow<int>(frameReader->frames()) });
	if (pointers.size() > 0)
		frameReader->read(pointers);
	for (const auto &channel : entireAudioFile) {
		float squaredSum{};
		for (const auto sample : channel)
			squaredSum += sample * sample;
		forProcessor.stimulusRms.push_back(std::sqrt(squaredSum / channel.size()));
	}
	frameReader->reset();

	audio.resize(frameReader->channels());

	frameProcessor = makeProcessor(forProcessor);

	device->closeStream();

	AudioDevice::StreamParameters forStreaming;
	forStreaming.framesPerBuffer = request.chunkSize;
	forStreaming.sampleRate = frameReader->sampleRate();
	forStreaming.channels = frameReader->channels();
	for (int i = 0; i < device->count(); ++i)
		if (device->description(i) == request.audioDevice)
			forStreaming.deviceIndex = i;

	device->openStream(forStreaming);
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
	device->setCallbackResultToContinue();
	device->startStream();
	if (device->failed())
		throw RequestFailure{ device->errorMessage() };
}

std::shared_ptr<AudioFrameReader> PlayAudioModel::makeReader(std::string filePath) {
	try {
		return readerFactory->make(filePath);
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

std::shared_ptr<AudioFrameProcessor> PlayAudioModel::makeProcessor(AudioFrameProcessorFactory::Parameters p) {
	try {
		return processorFactory->make(p);
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw RequestFailure{ e.what() };
	}
}

void PlayAudioModel::fillStreamBuffer(void * channels, int frames) {
	const auto pointers = static_cast<float **>(channels);
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio[i] = { pointers[i], frames };
		
	frameReader->read(audio);
	frameProcessor->process(audio);
	if (frameReader->complete())
		device->setCallbackResultToComplete();
}

std::vector<std::string> PlayAudioModel::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}
