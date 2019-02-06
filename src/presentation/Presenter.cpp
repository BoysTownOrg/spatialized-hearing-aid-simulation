#include "Presenter.h"

Presenter::Presenter(Model *model, View *view) :
	model{ model },
	view{ view }
{
	view->subscribe(this);
	view->populateAudioDeviceMenu(model->audioDeviceDescriptions());
	showTestSetupView();
	toggleSpatializationActivation();
	toggleHearingAidSimulationActivation();
}

void Presenter::showTestSetupView() {
	view->showSubjectId();
	view->showTesterId();
	view->showStimulusList();
	view->showTestFilePath();
	view->showConfirmButton();
	view->showBrirFilePath();
	view->showBrowseForBrirButton();
	view->showUsingSpatializationCheckBox();
	view->showLeftDslPrescriptionFilePath();
	view->showBrowseForLeftDslPrescriptionButton();
	view->showRightDslPrescriptionFilePath();
	view->showBrowseForRightDslPrescriptionButton();
	view->showAttack_ms();
	view->showRelease_ms();
	view->showChunkSize();
	view->showWindowSize();
	view->showUsingHearingAidSimulationCheckBox();
	view->showAudioFilePath();
	view->showPlayButton();
	view->showStopButton();
	view->showLevel_dB_Spl();
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

void Presenter::browseForAudioFile() {
	applyIfBrowseNotCancelled(
		view->browseForSavingFile({ "*.wav" }), 
		[=](std::string p) { this->view->setAudioFilePath(std::move(p)); }
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

void Presenter::browseForStimulusList() {
	applyIfBrowseNotCancelled(
		view->browseForDirectory(), 
		[=](std::string p) { this->view->setStimulusList(std::move(p)); }
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
	hideTestSetupView();
	showTesterView();
}

void Presenter::showTesterView() {
	view->showPlayNextTrialButton();
}

void Presenter::hideTestSetupView() {
	view->hideSubjectId();
	view->hideTesterId();
	view->hideStimulusList();
	view->hideTestFilePath();
	view->hideConfirmButton();
	view->hideBrirFilePath();
	view->hideBrowseForBrirButton();
	view->hideUsingSpatializationCheckBox();
	view->hideLeftDslPrescriptionFilePath();
	view->hideBrowseForLeftDslPrescriptionButton();
	view->hideRightDslPrescriptionFilePath();
	view->hideBrowseForRightDslPrescriptionButton();
	view->hideAttack_ms();
	view->hideRelease_ms();
	view->hideChunkSize();
	view->hideWindowSize();
	view->hideUsingHearingAidSimulationCheckBox();
	view->hideAudioFilePath();
	view->hidePlayButton();
	view->hideStopButton();
	view->hideLevel_dB_Spl();
}

Model::TestParameters Presenter::testParameters() {
	Model::TestParameters test;
	test.processing = processingParameters();
	test.testFilePath = view->testFilePath();
	test.audioDirectory = view->stimulusList();
	test.subjectId = view->subjectId();
	test.testerId = view->testerId();
	return test;
}

Model::ProcessingParameters Presenter::processingParameters() {
	Model::ProcessingParameters p;
	if (view->usingHearingAidSimulation()) {
		p.attack_ms = convertToDouble(view->attack_ms(), "attack time");
		p.release_ms = convertToDouble(view->release_ms(), "release time");
		p.chunkSize = convertToPositiveInteger(view->chunkSize(), "chunk size");
		p.windowSize = convertToPositiveInteger(view->windowSize(), "window size");
	}
	p.leftDslPrescriptionFilePath = view->leftDslPrescriptionFilePath();
	p.rightDslPrescriptionFilePath = view->rightDslPrescriptionFilePath();
	p.brirFilePath = view->brirFilePath();
	p.usingHearingAidSimulation = view->usingHearingAidSimulation();
	p.usingSpatialization = view->usingSpatialization();
	return p;
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

void Presenter::playNextTrial() {
	try {
		playTrial_();
	}
	catch (const std::runtime_error &failure) {
		view->showErrorDialog(failure.what());
	}
}

void Presenter::playTrial_() {
	model->playNextTrial(trialParameters());
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
		hideTesterView();
		showTestSetupView();
	}
}

void Presenter::hideTesterView() {
	view->hidePlayNextTrialButton();
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
	p.processing = processingParameters();
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
