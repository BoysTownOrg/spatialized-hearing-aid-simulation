#include "RecognitionTestModel.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <gsl/gsl>

RecognitionTestModel::RecognitionTestModel(
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

static std::vector<double> computeStimulusRms(const std::shared_ptr<AudioFrameReader> &reader) {
	std::vector<std::vector<float>> entireAudioFile(reader->channels());
	std::vector<gsl::span<float>> pointers;
	for (auto &channel : entireAudioFile) {
		channel.resize(gsl::narrow<std::vector<float>::size_type>(reader->frames()));
		pointers.push_back({ channel });
	}
	reader->read(pointers);
	std::vector<double> stimulusRms;
	for (const auto &channel : entireAudioFile) {
		float squaredSum{};
		for (const auto sample : channel)
			squaredSum += sample * sample;
		stimulusRms.push_back(std::sqrt(squaredSum / channel.size()));
	}
	return stimulusRms;
}

void RecognitionTestModel::play(PlayRequest request) {
	if (device->streaming())
		return;

	frameReader = makeReader(request.audioFilePath);

	AudioFrameProcessorFactory::Parameters forProcessor;
	forProcessor.channels = frameReader->channels();
	forProcessor.sampleRate = frameReader->sampleRate();
	forProcessor.stimulusRms = computeStimulusRms(frameReader);
	frameProcessor = makeProcessor(forProcessor);

	frameReader->reset();
	audio.resize(frameReader->channels());

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

std::shared_ptr<AudioFrameReader> RecognitionTestModel::makeReader(std::string filePath) {
	try {
		return readerFactory->make(filePath);
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

std::shared_ptr<AudioFrameProcessor> RecognitionTestModel::makeProcessor(AudioFrameProcessorFactory::Parameters p) {
	try {
		return processorFactory->make(p);
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

void RecognitionTestModel::initializeTest(TestParameters p) {
	AudioFrameProcessorFactory::Parameters forProcessor;
	forProcessor.attack_ms = p.attack_ms;
	forProcessor.release_ms = p.release_ms;
	forProcessor.brirFilePath = p.brirFilePath;
	forProcessor.leftDslPrescriptionFilePath = p.leftDslPrescriptionFilePath;
	forProcessor.rightDslPrescriptionFilePath = p.rightDslPrescriptionFilePath;
	forProcessor.level_dB_Spl = p.level_dB_Spl;
	forProcessor.chunkSize = p.chunkSize;
	forProcessor.windowSize = p.windowSize;
	makeProcessor(forProcessor);
	makeReader({});
	if (device->failed())
		throw TestInitializationFailure{ device->errorMessage() };
}

void RecognitionTestModel::playTrial() {
	if (device->streaming())
		return;
	device->closeStream();
	device->openStream({});
	device->startStream();
}

void RecognitionTestModel::fillStreamBuffer(void * channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio[i] = { static_cast<float **>(channels)[i], frames };
	frameReader->read(audio);
	frameProcessor->process(audio);
	if (frameReader->complete())
		device->setCallbackResultToComplete();
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}
