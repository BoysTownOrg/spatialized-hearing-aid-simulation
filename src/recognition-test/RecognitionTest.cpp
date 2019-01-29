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

void RecognitionTest::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return; 
	documentTrialParameters(std::move(p));
}

void RecognitionTest::playCalibration(CalibrationParameters) {
}

void RecognitionTest::playCalibration_(CalibrationParameters) {
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
