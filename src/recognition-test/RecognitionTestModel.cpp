#include "RecognitionTestModel.h"

RecognitionTestModel::RecognitionTestModel(
	StimulusList *list,
	StimulusPlayer *player
) :
	list{ list },
	player{ player } {}

bool RecognitionTestModel::testComplete() {
	return list->empty();
}

void RecognitionTestModel::initializeTest(TestParameters p) {
	list->initialize(p.audioDirectory);
	StimulusPlayer::Initialization init;
	init.attack_ms = p.attack_ms;
	init.brirFilePath = p.brirFilePath;
	init.chunkSize = p.chunkSize;
	init.leftDslPrescriptionFilePath = p.leftDslPrescriptionFilePath;
	init.release_ms = p.release_ms;
	init.rightDslPrescriptionFilePath = p.rightDslPrescriptionFilePath;
	init.windowSize = p.windowSize;

	// The hearing aid simulation in MATLAB used 119 dB SPL as a maximum.
	init.max_dB_Spl = 119;
	try {
		player->initialize(std::move(init));
	}
	catch (const StimulusPlayer::InitializationFailure &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

void RecognitionTestModel::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return; 
	playTrial_(std::move(p));
}

void RecognitionTestModel::playTrial_(Model::TrialParameters p) {
	try {
		playNextStimulus(p);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

void RecognitionTestModel::playNextStimulus(Model::TrialParameters p) {
	StimulusPlayer::PlayRequest request;
	request.audioFilePath = list->next();
	request.audioDevice = p.audioDevice;
	request.level_dB_Spl = p.level_dB_Spl;
	player->play(std::move(request));
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}

std::vector<int> RecognitionTestModel::preferredProcessingSizes() {
	return player->preferredProcessingSizes();
}
