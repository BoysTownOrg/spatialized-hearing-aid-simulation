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

class FormattedStream {
	std::stringstream stream{};
public:
	void insertFixed() {
		stream << std::fixed;
	}

	void insertPrecision(std::streamsize n) {
		stream << std::setprecision(n);
	}

	template<typename T>
	void insertLabeledParameterLine(std::string label, T p) {
		stream << std::move(label) << ": " << std::move(p) << '\n';
	}

	template<typename T>
	void insertIndentedLabeledParameterLine(std::string label, T p) {
		stream << "    ";
		insertLabeledParameterLine(std::move(label), std::move(p));
	}

	void insertLine(std::string s = {}) {
		stream << std::move(s) << '\n';
	}

	std::string str() {
		return stream.str();
	}
};

void RecognitionTestModel::documentTestParameters(TestParameters p) {
	FormattedStream stream;
	stream.insertLabeledParameterLine("subject", p.subjectId);
	stream.insertLabeledParameterLine("tester", p.testerId);
	stream.insertLine("DSL prescription");
	stream.insertIndentedLabeledParameterLine("left", p.leftDslPrescriptionFilePath);
	stream.insertIndentedLabeledParameterLine("right", p.rightDslPrescriptionFilePath);
	stream.insertLabeledParameterLine("BRIR", p.brirFilePath);
	stream.insertFixed();
	stream.insertPrecision(1);
	stream.insertLabeledParameterLine("attack (ms)", p.attack_ms);
	stream.insertLabeledParameterLine("release (ms)", p.release_ms);
	stream.insertLabeledParameterLine("window size (samples)", p.windowSize);
	stream.insertLabeledParameterLine("chunk size (samples)", p.chunkSize);
	stream.insertLine();
	documenter->write(stream.str());
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
	failedOnLastPlayRequest = false;
}

void RecognitionTestModel::playTrial_(TrialParameters p) {
	try {
		playNextStimulus(p);
	}
	catch (const StimulusPlayer::RequestFailure &e) {
		failedOnLastPlayRequest = true;
		throw TrialFailure{ e.what() };
	}
}

void RecognitionTestModel::playNextStimulus(TrialParameters p) {
	StimulusPlayer::PlayRequest request;
	if (failedOnLastPlayRequest)
		request.audioFilePath = currentStimulus_;
	else
		request.audioFilePath = currentStimulus_ = list->next();
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
