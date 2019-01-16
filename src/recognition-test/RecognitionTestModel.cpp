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
	StimulusPlayer::Initialization initialization;
	initialization.attack_ms = p.attack_ms;
	initialization.brirFilePath = p.brirFilePath;
	initialization.chunkSize = p.chunkSize;
	initialization.leftDslPrescriptionFilePath = p.leftDslPrescriptionFilePath;
	initialization.release_ms = p.release_ms;
	initialization.rightDslPrescriptionFilePath = p.rightDslPrescriptionFilePath;
	initialization.windowSize = p.windowSize;

	// The hearing aid simulation in MATLAB used 119 dB SPL as a maximum.
	initialization.max_dB_Spl = 119;
	try {
		player->initialize(initialization);
	}
	catch (const StimulusPlayer::InitializationFailure &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

void RecognitionTestModel::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return;
	StimulusPlayer::PlayRequest request;
	request.audioFilePath = list->next();
	request.audioDevice = p.audioDevice;
	request.level_dB_Spl = p.level_dB_Spl;
	try {
		player->play(request);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
