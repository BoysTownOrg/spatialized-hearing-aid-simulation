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
	forTest = std::move(p);
}

void RecognitionTestModel::playTrial(TrialParameters request) {
	try {
		StimulusPlayer::PlayRequest adapted;
		adapted.audioFilePath = list->next();
		adapted.attack_ms = forTest.attack_ms;
		adapted.audioDevice = request.audioDevice;
		adapted.brirFilePath = forTest.brirFilePath;
		adapted.chunkSize = forTest.chunkSize;
		adapted.leftDslPrescriptionFilePath = forTest.leftDslPrescriptionFilePath;
		adapted.level_dB_Spl = request.level_dB_Spl;
		adapted.release_ms = forTest.release_ms;
		adapted.rightDslPrescriptionFilePath = forTest.rightDslPrescriptionFilePath;
		adapted.windowSize = forTest.windowSize;
		player->play(adapted);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
