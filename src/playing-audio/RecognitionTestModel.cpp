#include "RecognitionTestModel.h"

RecognitionTestModel::RecognitionTestModel(
	StimulusList *list,
	StimulusPlayer *player
) :
	list{ list },
	player{ player }
{
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
