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
}

void RecognitionTestModel::play(PlayRequest request) {
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
	list->initialize(p.audioDirectory);
}

void RecognitionTestModel::playTrial(PlayRequest request) {
	StimulusPlayer::PlayRequest adapted;
	adapted.audioFilePath = list->next();
	player->play(adapted);
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return {};
}
