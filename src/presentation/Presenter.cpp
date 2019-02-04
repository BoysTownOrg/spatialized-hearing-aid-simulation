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
		prepareNewTest();
	}
	catch (const std::runtime_error &failure) {
		view->showErrorDialog(failure.what());
	}
}

void Presenter::prepareNewTest() {
	model->prepareNewTest(testParameters());
	view->hideTestSetup();
	view->showTesterView();
}

Model::TestParameters Presenter::testParameters() {
	Model::TestParameters test;
	test.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
	test.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
	test.brirFilePath = view->brirFilePath();
	if (view->usingHearingAidSimulation()) {
		test.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
		test.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
		test.release_ms = convertToDouble(view->release_ms(), "release time");
		test.attack_ms = convertToDouble(view->attack_ms(), "attack time");
	}
	test.usingSpatialization = view->usingSpatialization();
	test.usingHearingAidSimulation = view->usingHearingAidSimulation();
	test.testFilePath = view->testFilePath();
	test.audioDirectory = view->audioDirectory();
	test.subjectId = view->subjectId();
	test.testerId = view->testerId();
	return test;
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

static bool containsOnlyDigits(std::string s) noexcept {
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
	model->playTrial(trialParameters());
	switchViewIfTestComplete();
}

Model::TrialParameters Presenter::trialParameters() {
	Model::TrialParameters trial;
	trial.audioDevice = view->audioDevice();
	trial.level_dB_Spl = convertToDouble(view->level_dB_Spl(), "level");
	return trial;
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
		playCalibration_();
	}
	catch (const std::runtime_error &e) {
		view->showErrorDialog(e.what());
	}
}

void Presenter::playCalibration_() {
	Model::CalibrationParameters p;
	p.audioDevice = view->audioDevice();
	p.audioFilePath = view->audioFilePath();
	p.level_dB_Spl = convertToDouble(view->level_dB_Spl(), "level");
	if (view->usingHearingAidSimulation()) {
		p.processing.attack_ms = convertToDouble(view->attack_ms(), "attack time");
		p.processing.release_ms = convertToDouble(view->release_ms(), "release time");
		p.processing.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
		p.processing.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
		p.processing.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
		p.processing.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
		p.processing.brirFilePath = view->brirFilePath();
	}
	p.processing.usingHearingAidSimulation = view->usingHearingAidSimulation();
	p.processing.usingSpatialization = view->usingSpatialization();
	model->playCalibration(std::move(p));
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
