#include "RecognitionTest.h"

RecognitionTest::RecognitionTest(
	StimulusList *list,
	Documenter *documenter
) :
	list{ list },
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

void RecognitionTest::prepareNewTest_(TestParameters p) {
	initializeStimulusList(p.stimulusList);
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

void RecognitionTest::advanceTrial() {
	Documenter::TrialParameters p;
	p.stimulus = nextStimulus_;
	documenter->documentTrialParameters(p);
	nextStimulus_ = list->next();
}
