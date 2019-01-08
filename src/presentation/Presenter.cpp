#include "Presenter.h"

Presenter::Presenter(
	std::shared_ptr<Model> model, 
	std::shared_ptr<View> view
) :
	model{ std::move(model) },
	view{ std::move(view) }
{
	this->view->subscribe(this);
	this->view->populateAudioDeviceMenu(this->model->audioDeviceDescriptions());
}

void Presenter::run() {
	view->runEventLoop();
}

void Presenter::browseForLeftDslPrescription() {
	applyIfBrowseNotCancelled(
		view->browseForFile({ "*.json" }), 
		[=](std::string p) { this->view->setLeftDslPrescriptionFilePath(std::move(p)); }
	);
}

void Presenter::applyIfBrowseNotCancelled(std::string s, std::function<void(std::string)> f) {
	if (!view->browseCancelled())
		f(std::move(s));
}

void Presenter::browseForRightDslPrescription() {
	applyIfBrowseNotCancelled(
		view->browseForFile({ "*.json" }), 
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
		view->browseForFile({ "*.wav" }), 
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
	p.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
	p.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
	p.brirFilePath = view->brirFilePath();
	p.audioDirectory = view->audioDirectory();
	p.attack_ms = convertToDouble(view->attack_ms(), "attack time");
	p.release_ms = convertToDouble(view->release_ms(), "release time");
	p.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
	p.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
	model->initializeTest(p);
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
	try {
		return std::stoi(x);
	}
	catch (const std::invalid_argument &) {
		throw BadInput{ badInputMessage(x, identifier) };
	}
}

void Presenter::playTrial() {
	Model::TrialParameters p;
	p.audioDevice = view->audioDevice();
	p.level_dB_Spl = std::stod(view->level_dB_Spl());
	model->playTrial(p);
}

void Presenter::newTest() {
	view->showTestSetup();
}
