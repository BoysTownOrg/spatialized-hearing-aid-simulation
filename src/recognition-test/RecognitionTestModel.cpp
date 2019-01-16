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
	StimulusPlayer::Initialization initialization;
	initialization.attack_ms = forTest.attack_ms;
	initialization.brirFilePath = forTest.brirFilePath;
	initialization.chunkSize = forTest.chunkSize;
	initialization.leftDslPrescriptionFilePath = forTest.leftDslPrescriptionFilePath;
	initialization.release_ms = forTest.release_ms;
	initialization.rightDslPrescriptionFilePath = forTest.rightDslPrescriptionFilePath;
	initialization.windowSize = forTest.windowSize;

	// The hearing aid simulation in MATLAB used 119 dB SPL as a maximum.
	initialization.max_dB_Spl = 119;
	player->initialize(initialization);
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

		// The hearing aid simulation in MATLAB used 119 dB SPL as a maximum.
		request.max_dB_Spl = 119;

		player->play(request);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
