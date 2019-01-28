#include "Presenter.h"

Presenter::Presenter(Model *model, View *view) :
	model{ model },
	view{ view }
{
	view->subscribe(this);
	view->populateAudioDeviceMenu(model->audioDeviceDescriptions());
	view->showTestSetup();
	toggleSpatializationActivation();
	toggleHearingAidSimulationActivation();
}

void Presenter::toggleSpatializationActivation() {
	if (view->usingSpatialization())
		activateSpatialization();
	else
		deactivateSpatialization();
}

void Presenter::activateSpatialization() {
	view->activateBrowseForBrirButton();
	view->activateBrirFilePath();
}

void Presenter::deactivateSpatialization() {
	view->deactivateBrowseForBrirButton();
	view->deactivateBrirFilePath();
}

void Presenter::toggleHearingAidSimulationActivation() {
	if (view->usingHearingAidSimulation())
		activateHearingAidSimulation();
	else
		deactivateHearingAidSimulation();
}

void Presenter::activateHearingAidSimulation() {
	view->activateLeftDslPrescriptionFilePath();
	view->activateRightDslPrescriptionFilePath();
	view->activateBrowseForLeftDslPrescriptionButton();
	view->activateBrowseForRightDslPrescriptionButton();
	view->activateChunkSize();
	view->activateWindowSize();
	view->activateAttackTime_ms();
	view->activateReleaseTime_ms();
}

void Presenter::deactivateHearingAidSimulation() {
	view->deactivateLeftDslPrescriptionFilePath();
	view->deactivateRightDslPrescriptionFilePath();
	view->deactivateBrowseForLeftDslPrescriptionButton();
	view->deactivateBrowseForRightDslPrescriptionButton();
	view->deactivateChunkSize();
	view->deactivateWindowSize();
	view->deactivateAttackTime_ms();
	view->deactivateReleaseTime_ms();
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
	Model::TestParameters test;
	GlobalTestParameters global;
	global.subjectId = view->subjectId();
	global.testerId = view->testerId();
	global.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
	global.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
	global.brirFilePath = view->brirFilePath();
	if (view->usingHearingAidSimulation()) {
		global.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
		global.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
		global.release_ms = convertToDouble(view->release_ms(), "release time");
		global.attack_ms = convertToDouble(view->attack_ms(), "attack time");
	}
	global.usingSpatialization = view->usingSpatialization();
	global.usingHearingAidSimulation = view->usingHearingAidSimulation();
	test.testFilePath = view->testFilePath();
	test.audioDirectory = view->audioDirectory();
	test.global = &global;
	model->initializeTest(std::move(test));
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

void Presenter::calibrate() {
	view->showCalibration();
}

void Presenter::playCalibration() {
	try {
		Model::CalibrationParameters p;
		p.audioDevice = view->audioDevice();
		p.audioFilePath = view->audioFilePath();
		p.level_dB_Spl = convertToDouble(view->level_dB_Spl(), "level");
		model->playCalibration(std::move(p));
	}
	catch (const Model::CalibrationFailure & e) {
		view->showErrorDialog(e.what());
	}
}

void Presenter::stopCalibration() {
	model->stopCalibration();
}

void Presenter::toggleUsingSpatialization() {
	toggleSpatializationActivation();
}

void Presenter::toggleUsingHearingAidSimulation() {
	toggleHearingAidSimulationActivation();
}
