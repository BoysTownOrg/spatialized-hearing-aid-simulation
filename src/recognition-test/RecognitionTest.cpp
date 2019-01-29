#include "RecognitionTest.h"

RecognitionTest::RecognitionTest(
	StimulusList *list,
	StimulusPlayer *player,
	Documenter *documenter
) :
	list{ list },
	player{ player },
	documenter{ documenter } {}

bool RecognitionTest::testComplete() {
	return list->empty();
}

void RecognitionTest::prepareNewTest(TestParameters p) {
	try {
		prepareNewTest_(std::move(p));
	}
	catch (const std::runtime_error &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

void RecognitionTest::prepareNewTest_(TestParameters p) {
	initializeStimulusList(p.audioDirectory);
	initializeDocumenter(p.testFilePath);
	documentTestParameters(p);
}

void RecognitionTest::initializeStimulusList(std::string directory) {
	list->initialize(std::move(directory));
}

void RecognitionTest::initializeDocumenter(std::string testFilePath) {
	documenter->initialize(std::move(testFilePath));
}

void RecognitionTest::documentTestParameters(TestParameters p) {
	Documenter::TestParameters documenting;
	documenting.global = p.global;
	documenter->documentTestParameters(std::move(documenting));
}

void RecognitionTest::prepareNextTrial(TrialParameters p) {
	if (player->isPlaying())
		return; 
	prepareNextTrial_(p);
	documentTrialParameters(std::move(p));
	failedOnLastPlayRequest = false;
}

void RecognitionTest::prepareNextTrial_(TrialParameters p) {
	try {
		prepareNextStimulus(std::move(p));
	}
	catch (const StimulusPlayer::PreparationFailure &e) {
		failedOnLastPlayRequest = true;
		throw TrialFailure{ e.what() };
	}
}

void RecognitionTest::prepareNextStimulus(TrialParameters p) {
	StimulusPlayer::Preparation toPlay;
	if (!failedOnLastPlayRequest)
		currentStimulus_ = list->next();
	toPlay.audioDevice = p.audioDevice;
	toPlay.channels = 0;
	toPlay.framesPerBuffer = 0;
	toPlay.sampleRate = 0;
	player->prepareToPlay(std::move(toPlay));
}

void RecognitionTest::playTrial(TrialParameters) {

}

void RecognitionTest::playCalibration(CalibrationParameters p) {
	try {
		playCalibration_(std::move(p));
	}
	catch (const StimulusPlayer::PreparationFailure & e) {
		throw CalibrationFailure{ e.what() };
	}
}

void RecognitionTest::playCalibration_(CalibrationParameters p) {
	StimulusPlayer::Preparation request;
	request.audioDevice = p.audioDevice;
	player->prepareToPlay(std::move(request));
}

void RecognitionTest::stopCalibration() {
	player->stop();
}

void RecognitionTest::documentTrialParameters(TrialParameters p) {
	Documenter::TrialParameters documenting{};
	GlobalTrialParameters global;
	global.level_dB_Spl = p.level_dB_Spl;
	global.stimulus = currentStimulus_;
	documenting.global = &global;
	documenter->documentTrialParameters(std::move(documenting));
}

std::vector<std::string> RecognitionTest::audioDeviceDescriptions() {
	return player->audioDeviceDescriptions();
}
