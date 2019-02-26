#include "FltkView.h"
#include <FL/fl_ask.H>
#include <sstream>

Fl_ChoiceFacade::Fl_ChoiceFacade(int x, int y, int w, int h, const char *c) : 
	Fl_Choice{ x, y, w, h, c } {}

void Fl_ChoiceFacade::populate(std::vector<std::string> items) {
	for (auto s : items)
		add(s.c_str());
	value(0); // If the value is never set then FLTK crashes on read.
}

void FltkView::onBrowseTestFile(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForTestFile();
}

void FltkView::onBrowseLeftPrescription(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForLeftDslPrescription();
}

void FltkView::onBrowseRightPrescription(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForRightDslPrescription();
}

void FltkView::onBrowseStimulusList(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForStimulusList();
}

void FltkView::onBrowseAudio(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForAudioFile();
}

void FltkView::onBrowseBrir(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForBrir();
}

void FltkView::onConfirmTestSetup(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->confirmTestSetup();
}

void FltkView::onPlayTrial(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->playNextTrial();
}

void FltkView::onToggleSpatialization(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->toggleUsingSpatialization();
}

void FltkView::onToggleHearingAidSimulation(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->toggleUsingHearingAidSimulation();
}

void FltkView::onPlayCalibration(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->playCalibration();
}

void FltkView::onStopCalibration(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->stopCalibration();
}

void FltkView::onSaveAudio(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->saveAudio();
}

static void hideAllChildren(Fl_Group *parent)
{
	auto children = parent->children();
	for (auto i{ 0 }; i < children; ++i)
		parent->child(i)->hide();
}

FltkSpatialization::FltkSpatialization(int x, int y, int w, int h, const char *) :
	Fl_Group{ x, y, w, h },
	brirFilePath_{x + 100, y + 10, 200, 25, "BRIR file path" },
	browseBrir{x + 310, y + 10, 80, 25, "browse..." }
{
	end();
}

void FltkSpatialization::hide()
{
	hideAllChildren(this);
}

FltkHearingAidSimulationGroup::FltkHearingAidSimulationGroup(int x, int y, int w, int h, const char *) :
	Fl_Group{ x, y, w, h },
	leftPrescriptionFilePath_{x + 200, y + 10, 200, 25, "left DSL prescription file path" },
	browseLeftPrescription{x + 410, y + 10, 80, 25, "browse..." },
	rightPrescriptionFilePath_{x + 200, y + 35, 200, 25, "right DSL prescription file path" },
	browseRightPrescription{x + 410, y + 35, 80, 25, "browse..." },
	attack_ms_{x + 200, y + 60, 200, 25, "attack (ms)" },
	release_ms_{x + 200, y + 85, 200, 25, "release (ms)" },
	windowSize_{x + 200, y + 110, 200, 25, "window size (samples)" },
	chunkSize_{x + 200, y + 135, 200, 25, "chunk size (samples)" }
{
	end();
}

void FltkHearingAidSimulationGroup::hide()
{
	hideAllChildren(this);
}

FltkCalibration::FltkCalibration(int x, int y, int w, int h, const char *) :
	Fl_Group{ x, y, w, h },
	audioFilePath_{x + 100, y + 10, 200, 25, "audio file path"},
	browseForAudioFile{x + 310, y + 10, 80, 25, "browse..." },
	level_dB_Spl_{x + 100, y + 35, 200, 25, "level (dB SPL)" },
	play{ x + 25, y + 60, 60, 25, "play" },
	stop{ x + 125, y + 60, 60, 25, "stop" },
	save{ x + 225, y + 60, 60, 25, "save" }
{
	end();
}

void FltkCalibration::hide()
{
	hideAllChildren(this);
}

FltkWindow::FltkWindow(int x, int y, int w, int h, const char *):
	Fl_Double_Window{ x, y, w, h },
	subjectId_{ 100, 10, 200, 25, "subject ID" },
	testerId_{ 100, 35, 200, 25, "tester ID" },
	stimulusList_{ 100, 60, 200, 25, "stimulus list" },
	browseForStimulusList{310, 60, 80, 25, "browse..." },
	testFilePath_{100, 85, 200, 25, "test file path" },
	browseTestFilePath{310, 85, 80, 25, "browse..." },
	usingSpatialization_{ 425, 10, 18, 25, "spatialization" },
	spatialization{425, 35, 400, 45},
	usingHearingAidSimulation_{ 425, 80, 18, 25, "hearing aid simulation" },
	hearingAidSimulation{ 425, 105, 500, 170 },
	calibration{ 425, 275, 400, 95 },
	audioDevice_{100, 550, 200, 25, "audio device" },
	playNextTrial{350, 550, 100, 25, "play next trial" },
	confirm{850, 550, 60, 25, "confirm" }
{
	end();
}

FltkView::FltkView() :
	window{ 425, 300, 950, 600 }
{
	Fl::set_font(0, "Segoe UI");
	window.color(FL_LIGHT2);
	window.hearingAidSimulation.box(FL_ENGRAVED_BOX);
	window.hearingAidSimulation.color(FL_LIGHT2);
	window.spatialization.box(FL_ENGRAVED_BOX);
	window.spatialization.color(FL_LIGHT2);
	window.calibration.box(FL_ENGRAVED_BOX);
	window.calibration.color(FL_LIGHT2);
	registerCallbacks();
	turnOnSpatialization();
	turnOnHearingAidSimulation();
	populateChunkSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.hearingAidSimulation.chunkSize_.value(4);
	populateWindowSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.hearingAidSimulation.windowSize_.value(2);
	window.hearingAidSimulation.attack_ms_.value("5");
	window.hearingAidSimulation.release_ms_.value("50");
	window.calibration.level_dB_Spl_.value("65");
	hideAllChildren(&window);
	window.audioDevice_.show();
}

void FltkView::registerCallbacks() {
	window.browseTestFilePath.callback(onBrowseTestFile, this);
	window.hearingAidSimulation.browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	window.hearingAidSimulation.browseRightPrescription.callback(onBrowseRightPrescription, this);
	window.browseForStimulusList.callback(onBrowseStimulusList, this);
	window.calibration.browseForAudioFile.callback(onBrowseAudio, this);
	window.spatialization.browseBrir.callback(onBrowseBrir, this);
	window.confirm.callback(onConfirmTestSetup, this);
	window.usingSpatialization_.callback(onToggleSpatialization, this);
	window.usingHearingAidSimulation_.callback(onToggleHearingAidSimulation, this);
	window.playNextTrial.callback(onPlayTrial, this);
	window.calibration.play.callback(onPlayCalibration, this);
	window.calibration.stop.callback(onStopCalibration, this);
	window.calibration.save.callback(onSaveAudio, this);
}

void FltkView::turnOnHearingAidSimulation() {
	window.usingHearingAidSimulation_.value(1);
}

void FltkView::turnOnSpatialization() {
	window.usingSpatialization_.value(1);
}

void FltkView::hideSaveButton() {
	window.calibration.save.hide();
}

void FltkView::showSaveButton() {
	window.calibration.save.show();
}

void FltkView::hideSubjectId()
{
	window.subjectId_.hide();
}

void FltkView::hideTesterId()
{
	window.testerId_.hide();
}

void FltkView::hideStimulusList()
{
	window.stimulusList_.hide();
}

void FltkView::hideTestFilePath()
{
	window.testFilePath_.hide();
}

void FltkView::hideConfirmButton()
{
	window.confirm.hide();
}

void FltkView::hideBrirFilePath()
{
	window.spatialization.brirFilePath_.hide();
}

void FltkView::hideBrowseForBrirButton()
{
	window.spatialization.browseBrir.hide();
}

void FltkView::hideUsingSpatializationCheckBox()
{
	window.usingSpatialization_.hide();
}

void FltkView::hideLeftDslPrescriptionFilePath()
{
	window.hearingAidSimulation.leftPrescriptionFilePath_.hide();
}

void FltkView::hideBrowseForLeftDslPrescriptionButton()
{
	window.hearingAidSimulation.browseLeftPrescription.hide();
}

void FltkView::hideRightDslPrescriptionFilePath()
{
	window.hearingAidSimulation.rightPrescriptionFilePath_.hide();
}

void FltkView::hideBrowseForRightDslPrescriptionButton()
{
	window.hearingAidSimulation.browseRightPrescription.hide();
}

void FltkView::hideAttack_ms()
{
	window.hearingAidSimulation.attack_ms_.hide();
}

void FltkView::hideRelease_ms()
{
	window.hearingAidSimulation.release_ms_.hide();
}

void FltkView::hideChunkSize()
{
	window.hearingAidSimulation.chunkSize_.hide();
}

void FltkView::hideWindowSize()
{
	window.hearingAidSimulation.windowSize_.hide();
}

void FltkView::hideUsingHearingAidSimulationCheckBox()
{
	window.usingHearingAidSimulation_.hide();
}

void FltkView::hideAudioFilePath()
{
	window.calibration.audioFilePath_.hide();
}

void FltkView::hidePlayButton()
{
	window.calibration.play.hide();
}

void FltkView::hideStopButton()
{
	window.calibration.stop.hide();
}

void FltkView::hideLevel_dB_Spl()
{
	window.calibration.level_dB_Spl_.hide();
}

void FltkView::hidePlayNextTrialButton()
{
	window.playNextTrial.hide();
}

void FltkView::hideBrowseForAudioFileButton()
{
	window.calibration.browseForAudioFile.hide();
}

void FltkView::hideBrowseForStimulusListButton()
{
	window.browseForStimulusList.hide();
}

void FltkView::hideBrowseForTestFileButton()
{
	window.browseTestFilePath.hide();
}

void FltkView::showBrowseForTestFileButton()
{
	window.browseTestFilePath.show();
}

void FltkView::showBrowseForAudioFileButton()
{
	window.calibration.browseForAudioFile.show();
}

void FltkView::showBrowseForStimulusListButton()
{
	window.browseForStimulusList.show();
}

void FltkView::showSubjectId()
{
	window.subjectId_.show();
}

void FltkView::showTesterId()
{
	window.testerId_.show();
}

void FltkView::showStimulusList()
{
	window.stimulusList_.show();
}

void FltkView::showTestFilePath()
{
	window.testFilePath_.show();
}

void FltkView::showConfirmButton()
{
	window.confirm.show();
}

void FltkView::showBrirFilePath()
{
	window.spatialization.brirFilePath_.show();
}

void FltkView::showBrowseForBrirButton()
{
	window.spatialization.browseBrir.show();
}

void FltkView::showUsingSpatializationCheckBox()
{
	window.usingSpatialization_.show();
}

void FltkView::showLeftDslPrescriptionFilePath()
{
	window.hearingAidSimulation.leftPrescriptionFilePath_.show();
}

void FltkView::showBrowseForLeftDslPrescriptionButton()
{
	window.hearingAidSimulation.browseLeftPrescription.show();
}

void FltkView::showRightDslPrescriptionFilePath()
{
	window.hearingAidSimulation.rightPrescriptionFilePath_.show();
}

void FltkView::showBrowseForRightDslPrescriptionButton()
{
	window.hearingAidSimulation.browseRightPrescription.show();
}

void FltkView::showAttack_ms()
{
	window.hearingAidSimulation.attack_ms_.show();
}

void FltkView::showRelease_ms()
{
	window.hearingAidSimulation.release_ms_.show();
}

void FltkView::showChunkSize()
{
	window.hearingAidSimulation.chunkSize_.show();
}

void FltkView::showWindowSize()
{
	window.hearingAidSimulation.windowSize_.show();
}

void FltkView::showUsingHearingAidSimulationCheckBox()
{
	window.usingHearingAidSimulation_.show();
}

void FltkView::showAudioFilePath()
{
	window.calibration.audioFilePath_.show();
}

void FltkView::showPlayButton()
{
	window.calibration.play.show();
}

void FltkView::showStopButton()
{
	window.calibration.stop.show();
}

void FltkView::showLevel_dB_Spl()
{
	window.calibration.level_dB_Spl_.show();
}

void FltkView::showPlayNextTrialButton()
{
	window.playNextTrial.show();
}

void FltkView::deactivateBrowseForBrirButton() {
	window.spatialization.browseBrir.deactivate();
}

void FltkView::deactivateBrirFilePath() {
	window.spatialization.brirFilePath_.deactivate();
}

void FltkView::activateBrowseForBrirButton() {
	window.spatialization.browseBrir.activate();
}

void FltkView::activateBrirFilePath() {
	window.spatialization.brirFilePath_.activate();
}

void FltkView::activateLeftDslPrescriptionFilePath() {
	window.hearingAidSimulation.leftPrescriptionFilePath_.activate();
}

void FltkView::activateRightDslPrescriptionFilePath() {
	window.hearingAidSimulation.rightPrescriptionFilePath_.activate();
}

void FltkView::activateBrowseForLeftDslPrescriptionButton() {
	window.hearingAidSimulation.browseLeftPrescription.activate();
}

void FltkView::activateBrowseForRightDslPrescriptionButton() {
	window.hearingAidSimulation.browseRightPrescription.activate();
}

void FltkView::deactivateLeftDslPrescriptionFilePath() {
	window.hearingAidSimulation.leftPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateRightDslPrescriptionFilePath() {
	window.hearingAidSimulation.rightPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateBrowseForLeftDslPrescriptionButton() {
	window.hearingAidSimulation.browseLeftPrescription.deactivate();
}

void FltkView::deactivateBrowseForRightDslPrescriptionButton() {
	window.hearingAidSimulation.browseRightPrescription.deactivate();
}

void FltkView::activateReleaseTime_ms() {
	window.hearingAidSimulation.release_ms_.activate();
}

void FltkView::activateAttackTime_ms() {
	window.hearingAidSimulation.attack_ms_.activate();
}

void FltkView::activateWindowSize() {
	window.hearingAidSimulation.windowSize_.activate();
}

void FltkView::activateChunkSize() {
	window.hearingAidSimulation.chunkSize_.activate();
}

void FltkView::deactivateReleaseTime_ms() {
	window.hearingAidSimulation.release_ms_.deactivate();
}

void FltkView::deactivateAttackTime_ms() {
	window.hearingAidSimulation.attack_ms_.deactivate();
}

void FltkView::deactivateWindowSize() {
	window.hearingAidSimulation.windowSize_.deactivate();
}

void FltkView::deactivateChunkSize() {
	window.hearingAidSimulation.chunkSize_.deactivate();
}

void FltkView::populateAudioDeviceMenu(std::vector<std::string> items) {
	window.audioDevice_.populate(std::move(items));
}

void FltkView::populateChunkSizeMenu(std::vector<std::string> items) {
	window.hearingAidSimulation.chunkSize_.populate(std::move(items));
}

void FltkView::populateWindowSizeMenu(std::vector<std::string> items) {
	window.hearingAidSimulation.windowSize_.populate(std::move(items));
}

std::string FltkView::audioDevice() {
	return window.audioDevice_.text();
}

void FltkView::runEventLoop() {
	window.show();
	Fl::run();
}

void FltkView::subscribe(EventListener * e) {
	this->listener = e;
}

class Fl_Chooser_Facade {
	Fl_Native_File_Chooser chooser{};
public:
	auto type(Fl_Native_File_Chooser::Type t) {
		return chooser.type(t);
	}

	auto filter(std::vector<std::string> filters) {
		chooser.filter(formatFilters(std::move(filters)).c_str());
	}

	auto show() {
		return chooser.show();
	}

	auto filename() {
		return chooser.filename();
	}

	auto options(int opt) {
		return chooser.options(opt);
	}

private:
	std::string formatFilters(std::vector<std::string> filters) {
		std::stringstream stream;
		for (auto filter : filters)
			stream << filter << "\n";
		return stream.str();
	}
};

std::string FltkView::browseForOpeningFile(std::vector<std::string> filters) {
	Fl_Chooser_Facade chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
	chooser.filter(std::move(filters));
	browseResult = chooser.show();
	return chooser.filename();
}

enum class BrowseResult {
	cancelled = 1
};

bool FltkView::browseCancelled() {
	return browseResult == static_cast<int>(BrowseResult::cancelled);
}

std::string FltkView::browseForDirectory() {
	Fl_Chooser_Facade chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	browseResult = chooser.show();
	return chooser.filename();
}

std::string FltkView::browseForSavingFile(std::vector<std::string> filters) {
	Fl_Chooser_Facade chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	chooser.filter(std::move(filters));
	chooser.options(Fl_Native_File_Chooser::SAVEAS_CONFIRM | Fl_Native_File_Chooser::USE_FILTER_EXT);
	browseResult = chooser.show();
	return chooser.filename();
}

void FltkView::setTestFilePath(std::string p) {
	window.testFilePath_.value(p.c_str());
}

void FltkView::setStimulusList(std::string d) {
	window.stimulusList_.value(d.c_str());
}

void FltkView::setLeftDslPrescriptionFilePath(std::string p) {
	window.hearingAidSimulation.leftPrescriptionFilePath_.value(p.c_str());
}

void FltkView::setRightDslPrescriptionFilePath(std::string p) {
	window.hearingAidSimulation.rightPrescriptionFilePath_.value(p.c_str());
}

void FltkView::setBrirFilePath(std::string p) {
	window.spatialization.brirFilePath_.value(p.c_str());
}

void FltkView::setAudioFilePath(std::string p)
{
	window.calibration.audioFilePath_.value(p.c_str());
}

std::string FltkView::subjectId() {
	return window.subjectId_.value();
}

std::string FltkView::testerId() {
	return window.testerId_.value();
}

std::string FltkView::testFilePath() {
	return window.testFilePath_.value();
}

std::string FltkView::audioFilePath() {
	return window.calibration.audioFilePath_.value();
}

std::string FltkView::stimulusList() {
	return window.stimulusList_.value();
}

std::string FltkView::leftDslPrescriptionFilePath() {
	return window.hearingAidSimulation.leftPrescriptionFilePath_.value();
}

std::string FltkView::rightDslPrescriptionFilePath() {
	return window.hearingAidSimulation.rightPrescriptionFilePath_.value();
}

std::string FltkView::brirFilePath() {
	return window.spatialization.brirFilePath_.value();
}

std::string FltkView::level_dB_Spl() {
	return window.calibration.level_dB_Spl_.value();
}

std::string FltkView::attack_ms() {
	return window.hearingAidSimulation.attack_ms_.value();
}

std::string FltkView::release_ms() {
	return window.hearingAidSimulation.release_ms_.value();
}

std::string FltkView::windowSize() {
	return window.hearingAidSimulation.windowSize_.text();
}

std::string FltkView::chunkSize() {
	return window.hearingAidSimulation.chunkSize_.text();
}

bool FltkView::usingSpatialization() {
	return window.usingSpatialization_.value();
}

bool FltkView::usingHearingAidSimulation() {
	return window.usingHearingAidSimulation_.value();
}

void FltkView::showErrorDialog(std::string message) {
	fl_alert(message.c_str());
}
