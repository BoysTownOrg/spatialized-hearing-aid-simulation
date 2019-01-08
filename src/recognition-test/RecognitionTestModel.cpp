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

void RecognitionTestModel::playTrial(TrialParameters request) {
	try {
		StimulusPlayer::PlayRequest adapted;
		adapted.audioFilePath = list->next();
		player->play(adapted);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		throw RequestFailure{ e.what() };
	}
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return {};
}
