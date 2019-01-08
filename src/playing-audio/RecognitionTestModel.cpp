#include "RecognitionTestModel.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <gsl/gsl>

RecognitionTestModel::RecognitionTestModel(
	AudioDevice *device,
	AudioFrameReaderFactory *readerFactory,
	AudioFrameProcessorFactory *processorFactory,
	StimulusList *list,
	StimulusPlayer *player
) :
	device{ device },
	readerFactory{ readerFactory },
	processorFactory{ processorFactory },
	list{ list },
	player{ player }
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
	frameReader = makeReader(request.audioFilePath);

	AudioFrameProcessorFactory::Parameters forProcessor;
	forProcessor.stimulusRms = computeStimulusRms(frameReader);
	makeProcessor(forProcessor);

	frameReader->reset();
}

std::shared_ptr<AudioFrameReader> RecognitionTestModel::makeReader(std::string filePath) {
	try {
		return readerFactory->make(filePath);
	}
	catch (const AudioFrameReaderFactory::CreateError &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

std::shared_ptr<AudioFrameProcessor> RecognitionTestModel::makeProcessor(
	AudioFrameProcessorFactory::Parameters p
) {
	try {
		return processorFactory->make(p);
	}
	catch (const AudioFrameProcessorFactory::CreateError &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

bool RecognitionTestModel::testComplete() {
	return list->empty();
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
	frameProcessor = makeProcessor(forProcessor);
	makeReader({});
	list->initialize(p.audioDirectory);
}

void RecognitionTestModel::playTrial(PlayRequest request) {
	StimulusPlayer::PlayRequest adapted;
	adapted.audioFilePath = list->next();
	player->play(adapted);
	frameReader = makeReader({});
	
	audio.resize(frameReader->channels());
}

void RecognitionTestModel::fillStreamBuffer(void *channels, int frames) {
	for (decltype(audio)::size_type i = 0; i < audio.size(); ++i)
		audio[i] = { static_cast<float **>(channels)[i], frames };
	frameReader->read(audio);
	frameProcessor->process(audio);
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	std::vector<std::string> descriptions{};
	for (int i = 0; i < device->count(); ++i)
		descriptions.push_back(device->description(i));
	return descriptions;
}
