#include "Presenter.h"

Presenter::Presenter(Model *model, View *view) :
	model{ model },
	view{ view }
{
	view->subscribe(this);
	view->populateAudioDeviceMenu(model->audioDeviceDescriptions());
	const auto sizeItems = preferredProcessingSizes();
	view->populateChunkSizeMenu(sizeItems);
	view->populateWindowSizeMenu(sizeItems);
	view->showTestSetup();
}

std::vector<std::string> Presenter::preferredProcessingSizes() {
	std::vector<std::string> sizeItems{};
	for (auto size : model->preferredProcessingSizes())
		sizeItems.push_back(std::to_string(size));
	return sizeItems;
}

void Presenter::run() {
	view->runEventLoop();
}

void Presenter::browseForTestFile() {
	applyIfBrowseNotCancelled(
		view->browseForSavingFile({ "*.txt" }), 
		[=](std::string p) { this->view->setTestFilePath(std::move(p)); }
	);
}

void Presenter::browseForLeftDslPrescription() {
	applyIfBrowseNotCancelled(
		view->browseForOpeningFile({ "*.json" }), 
		[=](std::string p) { this->view->setLeftDslPrescriptionFilePath(std::move(p)); }
	);
}

void Presenter::applyIfBrowseNotCancelled(std::string s, std::function<void(std::string)> f) {
	if (!view->browseCancelled())
		f(std::move(s));
}

void Presenter::browseForRightDslPrescription() {
	applyIfBrowseNotCancelled(
		view->browseForOpeningFile({ "*.json" }), 
		[=](std::string p) { this->view->setRightDslPrescriptionFilePath(std::move(p)); }
	);
}

void Presenter::browseForAudio() {
	applyIfBrowseNotCancelled(
		view->browseForDirectory(), 
		[=](std::string p) { this->view->setAudioDirectory(std::move(p)); }
	);
}

void Presenter::browseForBrir() {
	applyIfBrowseNotCancelled(
		view->browseForOpeningFile({ "*.wav" }), 
		[=](std::string p) { this->view->setBrirFilePath(std::move(p)); }
	);
}

void Presenter::confirmTestSetup() {
	try {
		prepareTest();
	}
	catch (const std::runtime_error &failure) {
		view->showErrorDialog(failure.what());
	}
}

void Presenter::prepareTest() {
	initializeModel();
	view->hideTestSetup();
	view->showTesterView();
}

void Presenter::initializeModel() {
	Model::TestParameters p;
	p.subjectId = view->subjectId();
	p.testerId = view->testerId();
	p.testFilePath = view->testFilePath();
	p.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
	p.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
	p.brirFilePath = view->brirFilePath();
	p.audioDirectory = view->audioDirectory();
	p.attack_ms = convertToDouble(view->attack_ms(), "attack time");
	p.release_ms = convertToDouble(view->release_ms(), "release time");
	p.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
	p.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
	model->initializeTest(std::move(p));
}

static std::string badInputMessage(std::string x, std::string identifier) {
	return { "'" + x + "' is not a valid " + identifier + "." };
}

double Presenter::convertToDouble(
	std::string x, 
	std::string identifier
) {
	try {
		return std::stod(x);
	}
	catch (const std::invalid_argument &) {
		throw BadInput{ badInputMessage(x, identifier) };
	}
}

static bool containsOnlyDigits(std::string s) {
	return s.find_first_not_of("0123456789") == std::string::npos;
}

int Presenter::convertToPositiveInteger(std::string x, std::string identifier) {
	if (!containsOnlyDigits(x))
		throw BadInput{ badInputMessage(x, identifier) };
	return convertToInteger(x, identifier);
}

int Presenter::convertToInteger(std::string x, std::string identifier) {
	try {
		return std::stoi(x);
	}
	catch (const std::invalid_argument &) {
		throw BadInput{ badInputMessage(x, identifier) };
	}
}

void Presenter::playTrial() {
	try {
		playTrial_();
	}
	catch (const std::runtime_error &failure) {
		view->showErrorDialog(failure.what());
	}
}

void Presenter::playTrial_() {
	Model::TrialParameters p;
	p.audioDevice = view->audioDevice();
	p.level_dB_Spl = convertToDouble(view->level_dB_Spl(), "level");
	model->playTrial(std::move(p));
	switchViewIfTestComplete();
}

void Presenter::switchViewIfTestComplete() {
	if (model->testComplete()) {
		view->hideTesterView();
		view->showTestSetup();
	}
}

void Presenter::toggleUsingSpatialization()
{
}
