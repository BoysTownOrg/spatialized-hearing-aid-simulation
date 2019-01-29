#include "RecognitionTestModel.h"

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
	catch (const std::runtime_error &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

void RecognitionTestModel::initializeTest_(TestParameters p) {
	initializeStimulusList(p.audioDirectory);
	initializeDocumenter(p.testFilePath);
	documentTestParameters(p);
}

void RecognitionTestModel::initializeStimulusList(std::string directory) {
	list->initialize(std::move(directory));
}

void RecognitionTestModel::initializeDocumenter(std::string testFilePath) {
	documenter->initialize(std::move(testFilePath));
}

void RecognitionTestModel::documentTestParameters(TestParameters p) {
	Documenter::TestParameters documenting;
	documenting.global = p.global;
	documenter->documentTestParameters(std::move(documenting));
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
	catch (const StimulusPlayer::PreparationFailure &e) {
		failedOnLastPlayRequest = true;
		throw TrialFailure{ e.what() };
	}
}

void RecognitionTestModel::playNextStimulus(TrialParameters p) {
	StimulusPlayer::Preparation request;
	if (!failedOnLastPlayRequest)
		currentStimulus_ = list->next();
	request.audioFilePath = currentStimulus_;
	request.audioDevice = p.audioDevice;
	request.level_dB_Spl = p.level_dB_Spl;
	player->prepareToPlay(std::move(request));
}

void RecognitionTestModel::playCalibration(CalibrationParameters p) {
	try {
		playCalibration_(std::move(p));
	}
	catch (const StimulusPlayer::PreparationFailure & e) {
		throw CalibrationFailure{ e.what() };
	}
}

void RecognitionTestModel::playCalibration_(CalibrationParameters p) {
	StimulusPlayer::Preparation request;
	request.audioDevice = p.audioDevice;
	request.audioFilePath = p.audioFilePath;
	request.level_dB_Spl = p.level_dB_Spl;
	player->prepareToPlay(std::move(request));
}

void RecognitionTestModel::stopCalibration() {
	player->stop();
}

void RecognitionTestModel::documentTrialParameters(TrialParameters p) {
	Documenter::TrialParameters documenting{};
	GlobalTrialParameters global;
	global.level_dB_Spl = p.level_dB_Spl;
	global.stimulus = currentStimulus_;
	documenting.global = &global;
	documenter->documentTrialParameters(std::move(documenting));
}

std::vector<std::string> RecognitionTestModel::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
