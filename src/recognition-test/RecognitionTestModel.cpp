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

void RecognitionTestModel::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return;
	try {
		StimulusPlayer::PlayRequest request;
		request.audioFilePath = list->next();
		request.attack_ms = forTest.attack_ms;
		request.audioDevice = p.audioDevice;
		request.brirFilePath = forTest.brirFilePath;
		request.chunkSize = forTest.chunkSize;
		request.leftDslPrescriptionFilePath = forTest.leftDslPrescriptionFilePath;
		request.level_dB_Spl = p.level_dB_Spl;
		request.release_ms = forTest.release_ms;
		request.rightDslPrescriptionFilePath = forTest.rightDslPrescriptionFilePath;
		request.windowSize = forTest.windowSize;
		player->play(request);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
