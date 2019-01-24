#include "RecognitionTestModel.h"
#include <sstream>
#include <iomanip>

RecognitionTestModel::RecognitionTestModel(
	StimulusList *list,
	StimulusPlayer *player,
	Documenter *documenter
) :
	list{ list },
	player{ player },
	documenter{ documenter } {}

bool RecognitionTestModel::testComplete() {
	return list->empty();
}

void RecognitionTestModel::initializeTest(TestParameters p) {
	try {
		initializeTest_(std::move(p));
	}
	catch (const StimulusPlayer::InitializationFailure &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

void RecognitionTestModel::initializeTest_(TestParameters p) {
	initializeStimulusList(p.audioDirectory);
	initializeDocumenter(p.testFilePath);
	documentTestParameters(p);
	initializeStimulusPlayer(std::move(p));
}

void RecognitionTestModel::initializeStimulusList(std::string directory) {
	list->initialize(std::move(directory));
}

void RecognitionTestModel::initializeDocumenter(std::string testFilePath) {
	documenter->initialize(std::move(testFilePath));
	if (documenter->failed())
		throw TestInitializationFailure{ documenter->errorMessage() };
}

void RecognitionTestModel::documentTestParameters(TestParameters p) {
	Documenter::TestParameters adapted;
	adapted.global = p.global;
	documenter->documentTestParameters(adapted);
}

void RecognitionTestModel::initializeStimulusPlayer(TestParameters p) {
	StimulusPlayer::Initialization init;
	init.global = p.global;
	player->initialize(std::move(init));
}

void RecognitionTestModel::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return; 
	playTrial_(p);
	documentTrialParameters(std::move(p));
	failedOnLastPlayRequest = false;
}

void RecognitionTestModel::playTrial_(TrialParameters p) {
	try {
		playNextStimulus(std::move(p));
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		failedOnLastPlayRequest = true;
		throw TrialFailure{ e.what() };
	}
}

void RecognitionTestModel::playNextStimulus(TrialParameters p) {
	StimulusPlayer::PlayRequest request;
	if (!failedOnLastPlayRequest)
		currentStimulus_ = list->next();
	request.audioFilePath = currentStimulus_;
	request.audioDevice = p.audioDevice;
	request.level_dB_Spl = p.level_dB_Spl;
	player->play(std::move(request));
}

void RecognitionTestModel::documentTrialParameters(TrialParameters) {
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}

std::vector<int> RecognitionTestModel::preferredProcessingSizes() {
	return player->preferredProcessingSizes();
}
