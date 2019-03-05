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
	view->testSetup()->show();
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
		[=](std::string p) { view->testSetup()->setTestFilePath(std::move(p)); }
	);
}

void Presenter::browseForAudioFile() {
	applyIfBrowseNotCancelled(
		view->browseForOpeningFile({ "*.wav" }), 
		[=](std::string p) { view->testSetup()->setAudioFilePath(std::move(p)); }
	);
}

void Presenter::browseForLeftDslPrescription() {
	applyIfBrowseNotCancelled(
		view->browseForOpeningFile({ "*.json" }), 
		[=](std::string p) { view->testSetup()->setLeftDslPrescriptionFilePath(std::move(p)); }
	);
}

void Presenter::applyIfBrowseNotCancelled(std::string s, std::function<void(std::string)> f) {
	if (!view->browseCancelled())
		f(std::move(s));
}

void Presenter::browseForRightDslPrescription() {
	applyIfBrowseNotCancelled(
		view->browseForOpeningFile({ "*.json" }), 
		[=](std::string p) { view->testSetup()->setRightDslPrescriptionFilePath(std::move(p)); }
	);
}

void Presenter::browseForStimulusList() {
	applyIfBrowseNotCancelled(
		view->browseForDirectory(), 
		[=](std::string p) { view->testSetup()->setStimulusList(std::move(p)); }
	);
}

void Presenter::browseForBrir() {
	applyIfBrowseNotCancelled(
		view->browseForOpeningFile({ "*.wav" }), 
		[=](std::string p) { view->testSetup()->setBrirFilePath(std::move(p)); }
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
	auto p = testing();
	model->prepareNewTest(&p);
	hideTestSetupView();
	showTesterView();
}

void Presenter::showTesterView() {
	view->showPlayNextTrialButton();
}

void Presenter::hideTestSetupView() {
	view->testSetup()->hide();
}

Model::Testing Presenter::testing() {
	Model::Testing testing_;
	testing_.processing = signalProcessing();
	testing_.testFilePath = view->testSetup()->testFilePath();
	testing_.audioDirectory = view->testSetup()->stimulusList();
	testing_.subjectId = view->testSetup()->subjectId();
	testing_.testerId = view->testSetup()->testerId();
	return testing_;
}

Model::SignalProcessing Presenter::signalProcessing() {
	Model::SignalProcessing p;
	if (view->usingHearingAidSimulation()) {
		p.attack_ms = convertToDouble(view->testSetup()->attack_ms(), "attack time");
		p.release_ms = convertToDouble(view->testSetup()->release_ms(), "release time");
		p.chunkSize = convertToPositiveInteger(view->testSetup()->chunkSize(), "chunk size");
		p.windowSize = convertToPositiveInteger(view->testSetup()->windowSize(), "window size");
	}
	p.leftDslPrescriptionFilePath = view->testSetup()->leftDslPrescriptionFilePath();
	p.rightDslPrescriptionFilePath = view->testSetup()->rightDslPrescriptionFilePath();
	p.brirFilePath = view->testSetup()->brirFilePath();
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
	auto p = trial();
	model->playNextTrial(&p);
	switchViewIfTestComplete();
}

Model::Trial Presenter::trial() {
	Model::Trial trial_;
	trial_.audioDevice = view->audioDevice();
	trial_.level_dB_Spl = convertToDouble(view->testSetup()->level_dB_Spl(), "level");
	return trial_;
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

void Presenter::saveAudio() {
	try {
		saveAudio_();
	}
	catch (const std::runtime_error &e) {
		view->showErrorDialog(e.what());
	}
}

void Presenter::saveAudio_() {
	Model::SavingAudio saving_;
	saving_.inputAudioFilePath = view->testSetup()->audioFilePath();
	saving_.level_dB_Spl = convertToDouble(view->testSetup()->level_dB_Spl(), "level");
	saving_.processing = signalProcessing();
	model->processAudioForSaving(saving_);
	auto save = view->browseForSavingFile({ "*.wav" });
	if (!view->browseCancelled())
		model->saveAudio(save);
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
	Model::Calibration calibration_;
	calibration_.audioDevice = view->audioDevice();
	calibration_.audioFilePath = view->testSetup()->audioFilePath();
	calibration_.level_dB_Spl = convertToDouble(view->testSetup()->level_dB_Spl(), "level");
	calibration_.processing = signalProcessing();
	model->playCalibration(&calibration_);
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
