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

std::string RecognitionTest::nextStimulus() {
	return nextStimulus_;
}

void RecognitionTest::prepareNewTest(TestParameters p) {
	try {
		prepareNewTest_(std::move(p));
	}
	catch (const std::runtime_error &e) {
		throw TestInitializationFailure{ e.what() };
	}
}

void RecognitionTest::prepareNextTrial(TrialParameters)
{
}

void RecognitionTest::prepareNewTest_(TestParameters p) {
	initializeStimulusList(p.audioDirectory);
	initializeDocumenter(p.testFilePath);
	documentTestParameters(p);
	nextStimulus_ = list->next();
}

void RecognitionTest::initializeStimulusList(std::string directory) {
	list->initialize(std::move(directory));
}

void RecognitionTest::initializeDocumenter(std::string testFilePath) {
	documenter->initialize(std::move(testFilePath));
}

void RecognitionTest::documentTestParameters(TestParameters) {
	documenter->documentTestParameters({});
}

void RecognitionTest::playNextTrial() {
	if (player->isPlaying())
		return; 
	documentTrialParameters({});
	nextStimulus_ = list->next();
}

void RecognitionTest::documentTrialParameters(TrialParameters p) {
	Documenter::TrialParameters documenting{};
	GlobalTrialParameters global;
	global.level_dB_Spl = p.level_dB_Spl;
	documenting.global = &global;
	documenter->documentTrialParameters(std::move(documenting));
}
