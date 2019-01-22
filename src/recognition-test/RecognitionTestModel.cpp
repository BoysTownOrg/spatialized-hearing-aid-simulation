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

class FormattedStream {
	std::stringstream stream{};
public:
	void putFixed() {
		stream << std::fixed;
	}

	void putPrecision(std::streamsize n) {
		stream << std::setprecision(n);
	}

	template<typename T>
	void putLabeledParameter(std::string label, T p) {
		stream << label << ": " << p;
	}

	template<typename T>
	void putIndentedLabeledParameter(std::string label, T p) {
		stream << "    " << label << ": " << p;
	}

	void putLine(std::string s) {
		stream << s << '\n';
	}

	std::string str() {
		return stream.str();
	}
};

void RecognitionTestModel::documentTestParameters(TestParameters p) {
	FormattedStream stream;
	stream.putLine("DSL prescription");
	stream.putIndentedLabeledParameter("left", p.leftDslPrescriptionFilePath);
	stream.putLine("");
	stream.putIndentedLabeledParameter("right", p.rightDslPrescriptionFilePath);
	stream.putLine("");
	stream.putLabeledParameter("BRIR", p.brirFilePath);
	stream.putLine("");
	stream.putFixed();
	stream.putPrecision(1);
	stream.putLabeledParameter("attack (ms)", p.attack_ms);
	stream.putLine("");
	stream.putLabeledParameter("release (ms)", p.release_ms);
	stream.putLine("");
	stream.putLabeledParameter("window size (samples)", p.windowSize);
	stream.putLine("");
	stream.putLabeledParameter("chunk size (samples)", p.chunkSize);
	stream.putLine("");
	documenter->writeLine(stream.str());
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
