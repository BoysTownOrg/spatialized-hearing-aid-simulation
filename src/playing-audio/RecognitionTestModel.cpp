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
