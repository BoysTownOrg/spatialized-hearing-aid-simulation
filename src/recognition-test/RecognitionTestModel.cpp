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
	documentTestParameters(p);
	initializeDocumenter(p.testFilePath);
	initializeStimulusPlayer(std::move(p));
}

void RecognitionTestModel::initializeStimulusList(std::string directory) {
	list->initialize(std::move(directory));
}

void RecognitionTestModel::initializeDocumenter(std::string testFilePath) {
	documenter->initialize(std::move(testFilePath));
}

void RecognitionTestModel::documentTestParameters(TestParameters p) {
	documenter->writeLine("DSL prescription");
	documenter->writeLine("    left: " + p.leftDslPrescriptionFilePath);
	documenter->writeLine("    right: " + p.rightDslPrescriptionFilePath);
	documenter->writeLine("BRIR: " + p.brirFilePath);
	std::stringstream stream;
	stream << "attack (ms): ";
	stream << std::fixed;
	stream << std::setprecision(1);
	stream << p.attack_ms;
	stream << '\n';
	stream << "release (ms): ";
	stream << p.release_ms;
	documenter->writeLine(stream.str());
	documenter->writeLine("window size (samples): " + std::to_string(p.windowSize));
	documenter->writeLine("chunk size (samples): " + std::to_string(p.chunkSize));
	documenter->writeLine("");
}

void RecognitionTestModel::initializeStimulusPlayer(TestParameters p) {
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

	player->initialize(std::move(init));
}

void RecognitionTestModel::playTrial(TrialParameters p) {
	if (player->isPlaying())
		return; 
	playTrial_(std::move(p));
}

void RecognitionTestModel::playTrial_(TrialParameters p) {
	try {
		playNextStimulus(p);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		throw TrialFailure{ e.what() };
	}
}

void RecognitionTestModel::playNextStimulus(TrialParameters p) {
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
