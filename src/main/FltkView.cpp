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

void FltkView::onBrowseAudio(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForStimulusList();
}

void FltkView::onBrowseBrir(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->browseForBrir();
}

void FltkView::onConfirmTestSetup(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->confirmTestSetup();
}

void FltkView::onPlayTrial(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->playTrial();
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

FltkWindow::FltkWindow(int x, int y, int w, int h, const char *):
	Fl_Double_Window{ x, y, w, h },
	subjectId_{ 100, 10, 200, 45, "subject ID" },
	testerId_{ 100, 60, 200, 45, "tester ID" },
	audioDirectory_{ 100, 110, 200, 45, "audio directory" },
	browseAudio{310, 110, 60, 45, "browse" },
	usingSpatialization_{ 50, 200, 60, 45 },
	brirFilePath_{100, 200, 200, 45, "BRIR file path" },
	browseBrir{310, 200, 60, 45, "browse" },
	usingHearingAidSimulation_{ 450, 200, 60, 45 },
	leftPrescriptionFilePath_{700, 200, 200, 45, "left DSL prescription file path" },
	browseLeftPrescription{910, 200, 60, 45, "browse" },
	rightPrescriptionFilePath_{700, 250, 200, 45, "right DSL prescription file path" },
	browseRightPrescription{910, 250, 60, 45, "browse" },
	attack_ms_{700, 300, 200, 45, "attack (ms)" },
	release_ms_{700, 350, 200, 45, "release (ms)" },
	windowSize_{700, 400, 200, 45, "window size (samples)" },
	chunkSize_{700, 450, 200, 45, "chunk size (samples)" },
	audioFilePath_{100, 300, 200, 45, "audio file path"},
	level_dB_Spl_{100, 350, 200, 45, "level (dB SPL)" },
	play{ 50, 400, 60, 45, "play" },
	stop{ 150, 400, 60, 45, "stop" },
	testFilePath_{500, 550, 200, 45, "test file path" },
	browseTestFilePath{710, 550, 60, 45, "browse" },
	audioDevice_{100, 550, 200, 45, "audio device" },
	playNextTrial{350, 550, 60, 45, "play next trial" },
	confirm{850, 550, 60, 45, "confirm" }
{
	end();
}

FltkView::FltkView() :
	window{ 425, 300, 950, 600 }
{
	window.show();
	registerCallbacks();
	turnOnSpatialization();
	turnOnHearingAidSimulation();
	populateChunkSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.chunkSize_.value(4);
	populateWindowSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.windowSize_.value(2);
	window.attack_ms_.value("5");
	window.release_ms_.value("50");
	window.level_dB_Spl_.value("65");
}

void FltkView::registerCallbacks() {
	window.browseTestFilePath.callback(onBrowseTestFile, this);
	window.browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	window.browseRightPrescription.callback(onBrowseRightPrescription, this);
	window.browseAudio.callback(onBrowseAudio, this);
	window.browseBrir.callback(onBrowseBrir, this);
	window.confirm.callback(onConfirmTestSetup, this);
	window.usingSpatialization_.callback(onToggleSpatialization, this);
	window.usingHearingAidSimulation_.callback(onToggleHearingAidSimulation, this);
	window.playNextTrial.callback(onPlayTrial, this);
	window.play.callback(onPlayCalibration, this);
	window.stop.callback(onStopCalibration, this);
}

void FltkView::turnOnHearingAidSimulation() {
	window.usingHearingAidSimulation_.value(1);
}

void FltkView::turnOnSpatialization() {
	window.usingSpatialization_.value(1);
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
	window.audioDirectory_.hide();
}

void FltkView::hideOutputFilePath()
{
	window.testFilePath_.hide();
}

void FltkView::hideConfirmButton()
{
	window.confirm.hide();
}

void FltkView::hideBrirFilePath()
{
	window.brirFilePath_.hide();
}

void FltkView::hideBrowseForBrirButton()
{
	window.browseBrir.hide();
}

void FltkView::hideUsingSpatializationCheckBox()
{
	window.usingSpatialization_.hide();
}

void FltkView::hideLeftDslPrescriptionFilePath()
{
	window.leftPrescriptionFilePath_.hide();
}

void FltkView::hideBrowseForLeftDslPrescriptionButton()
{
	window.browseLeftPrescription.hide();
}

void FltkView::hideRightDslPrescriptionFilePath()
{
	window.rightPrescriptionFilePath_.hide();
}

void FltkView::hideBrowseForRightDslPrescriptionButton()
{
	window.browseRightPrescription.hide();
}

void FltkView::hideAttack_ms()
{
	window.attack_ms_.hide();
}

void FltkView::hideRelease_ms()
{
	window.release_ms_.hide();
}

void FltkView::hideChunkSize()
{
	window.chunkSize_.hide();
}

void FltkView::hideWindowSize()
{
	window.windowSize_.hide();
}

void FltkView::hideUsingHearingAidSimulationCheckBox()
{
	window.usingHearingAidSimulation_.hide();
}

void FltkView::hideAudioFilePath()
{
	window.audioFilePath_.hide();
}

void FltkView::hidePlayButton()
{
	window.play.hide();
}

void FltkView::hideStopButton()
{
	window.stop.hide();
}

void FltkView::hideLevel_dB_Spl()
{
	window.level_dB_Spl_.hide();
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
	window.audioDirectory_.show();
}

void FltkView::showOutputFilePath()
{
	window.testFilePath_.show();
}

void FltkView::showConfirmButton()
{
	window.confirm.show();
}

void FltkView::showBrirFilePath()
{
	window.brirFilePath_.show();
}

void FltkView::showBrowseForBrirButton()
{
	window.browseBrir.show();
}

void FltkView::showUsingSpatializationCheckBox()
{
	window.usingSpatialization_.show();
}

void FltkView::showLeftDslPrescriptionFilePath()
{
	window.leftPrescriptionFilePath_.show();
}

void FltkView::showBrowseForLeftDslPrescriptionButton()
{
	window.browseLeftPrescription.show();
}

void FltkView::showRightDslPrescriptionFilePath()
{
	window.rightPrescriptionFilePath_.show();
}

void FltkView::showBrowseForRightDslPrescriptionButton()
{
	window.browseRightPrescription.show();
}

void FltkView::showAttack_ms()
{
	window.attack_ms_.show();
}

void FltkView::showRelease_ms()
{
	window.release_ms_.show();
}

void FltkView::showChunkSize()
{
	window.chunkSize_.show();
}

void FltkView::showWindowSize()
{
	window.windowSize_.show();
}

void FltkView::showUsingHearingAidSimulationCheckBox()
{
	window.usingHearingAidSimulation_.show();
}

void FltkView::showAudioFilePath()
{
	window.audioFilePath_.show();
}

void FltkView::showPlayButton()
{
	window.play.show();
}

void FltkView::showStopButton()
{
	window.stop.show();
}

void FltkView::showLevel_dB_Spl()
{
	window.level_dB_Spl_.show();
}

void FltkView::showTesterView() {
}

void FltkView::hideTesterView() {
}

void FltkView::deactivateBrowseForBrirButton() {
	window.browseBrir.deactivate();
}

void FltkView::deactivateBrirFilePath() {
	window.brirFilePath_.deactivate();
}

void FltkView::activateBrowseForBrirButton() {
	window.browseBrir.activate();
}

void FltkView::activateBrirFilePath() {
	window.brirFilePath_.activate();
}

void FltkView::activateLeftDslPrescriptionFilePath() {
	window.leftPrescriptionFilePath_.activate();
}

void FltkView::activateRightDslPrescriptionFilePath() {
	window.rightPrescriptionFilePath_.activate();
}

void FltkView::activateBrowseForLeftDslPrescriptionButton() {
	window.browseLeftPrescription.activate();
}

void FltkView::activateBrowseForRightDslPrescriptionButton() {
	window.browseRightPrescription.activate();
}

void FltkView::deactivateLeftDslPrescriptionFilePath() {
	window.leftPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateRightDslPrescriptionFilePath() {
	window.rightPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateBrowseForLeftDslPrescriptionButton() {
	window.browseLeftPrescription.deactivate();
}

void FltkView::deactivateBrowseForRightDslPrescriptionButton() {
	window.browseRightPrescription.deactivate();
}

void FltkView::activateReleaseTime_ms() {
	window.release_ms_.activate();
}

void FltkView::activateAttackTime_ms() {
	window.attack_ms_.activate();
}

void FltkView::activateWindowSize() {
	window.windowSize_.activate();
}

void FltkView::activateChunkSize() {
	window.chunkSize_.activate();
}

void FltkView::deactivateReleaseTime_ms() {
	window.release_ms_.deactivate();
}

void FltkView::deactivateAttackTime_ms() {
	window.attack_ms_.deactivate();
}

void FltkView::deactivateWindowSize() {
	window.windowSize_.deactivate();
}

void FltkView::deactivateChunkSize() {
	window.chunkSize_.deactivate();
}

void FltkView::populateAudioDeviceMenu(std::vector<std::string> items) {
	window.audioDevice_.populate(std::move(items));
}

void FltkView::populateChunkSizeMenu(std::vector<std::string> items) {
	window.chunkSize_.populate(std::move(items));
}

void FltkView::populateWindowSizeMenu(std::vector<std::string> items) {
	window.windowSize_.populate(std::move(items));
}

std::string FltkView::audioDevice() {
	return window.audioDevice_.text();
}

void FltkView::runEventLoop() {
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

void FltkView::setAudioDirectory(std::string d) {
	window.audioDirectory_.value(d.c_str());
}

void FltkView::setLeftDslPrescriptionFilePath(std::string p) {
	window.leftPrescriptionFilePath_.value(p.c_str());
}

void FltkView::setRightDslPrescriptionFilePath(std::string p) {
	window.rightPrescriptionFilePath_.value(p.c_str());
}

void FltkView::setBrirFilePath(std::string p) {
	window.brirFilePath_.value(p.c_str());
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
	return window.audioFilePath_.value();
}

std::string FltkView::audioDirectory() {
	return window.audioDirectory_.value();
}

std::string FltkView::leftDslPrescriptionFilePath() {
	return window.leftPrescriptionFilePath_.value();
}

std::string FltkView::rightDslPrescriptionFilePath() {
	return window.rightPrescriptionFilePath_.value();
}

std::string FltkView::brirFilePath() {
	return window.brirFilePath_.value();
}

std::string FltkView::level_dB_Spl() {
	return window.level_dB_Spl_.value();
}

std::string FltkView::attack_ms() {
	return window.attack_ms_.value();
}

std::string FltkView::release_ms() {
	return window.release_ms_.value();
}

std::string FltkView::windowSize() {
	return window.windowSize_.text();
}

std::string FltkView::chunkSize() {
	return window.chunkSize_.text();
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
