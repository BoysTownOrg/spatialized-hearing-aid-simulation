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
	static_cast<FltkView *>(self)->listener->browseForAudio();
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

FltkCalibrationView::FltkCalibrationView(int x, int y, int w, int h, const char *):
	Fl_Group{ x, y, w, h },
	audioFilePath_{250, 300, 200, 45, "audio file path"},
	play{ 50, 600, 60, 45, "play" },
	stop{ 150, 600, 60, 45, "stop" }
{
}

FltkSetupView::FltkSetupView(int x, int y, int w, int h, const char *) :
	Fl_Group{ x, y, w, h },
	testFilePath_(250, 50, 200, 45, "test file path"),
	browseTestFilePath(460, 50, 60, 45, "browse"),
	subjectId_(250, 100, 200, 45, "subject ID"),
	testerId_(250, 150, 200, 45, "tester ID"),
	audioDirectory_(250, 200, 200, 45, "audio directory"),
	browseAudio(460, 200, 60, 45, "browse"),
	brirFilePath_(250, 250, 200, 45, "BRIR file path"),
	browseBrir(460, 250, 60, 45, "browse"),
	usingSpatialization_(100, 250, 60, 45),
	leftPrescriptionFilePath_(250, 300, 200, 45, "left DSL prescription file path"),
	browseLeftPrescription(460, 300, 60, 45, "browse"),
	usingHearingAidSimulation_(20, 300, 60, 45),
	rightPrescriptionFilePath_(250, 350, 200, 45, "right DSL prescription file path"),
	browseRightPrescription(460, 350, 60, 45, "browse"),
	attack_ms_(250, 400, 200, 45, "attack (ms)"),
	release_ms_(250, 450, 200, 45, "release (ms)"),
	windowSize_(250, 500, 200, 45, "window size (samples)"),
	chunkSize_(250, 550, 200, 45, "chunk size (samples)"),
	confirm(250, 600, 60, 45, "confirm")
{
	end();
}

FltkTesterView::FltkTesterView(int x, int y, int w, int h, const char *) :
	Fl_Group{ x, y, w, h },
	level_dB_Spl_(250, 250, 200, 45, "level (dB SPL)"),
	audioDevice_(250, 500, 200, 45, "audio device"),
	play(250, 550, 60, 45, "play next trial")
{
	end();
}

FltkWindow::FltkWindow(int x, int y, int w, int h, const char *):
	Fl_Double_Window{ x, y, w, h },
	testerView{ 0, 0, 600, 700 },
	setupView{ 0, 0, 600, 700 },
	calibrationView{ 0, 0, 600, 700 }
{
	end();
}

FltkView::FltkView() :
	window{ 800, 200, 600, 700 }
{
	window.show();
	hideTesterView();
	registerCallbacks();
	turnOnSpatialization();
	turnOnHearingAidSimulation();
	populateChunkSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.setupView.chunkSize_.value(4);
	populateWindowSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.setupView.windowSize_.value(2);
	window.setupView.attack_ms_.value("5");
	window.setupView.release_ms_.value("50");
	window.testerView.level_dB_Spl_.value("65");
}

void FltkView::registerCallbacks() {
	window.setupView.browseTestFilePath.callback(onBrowseTestFile, this);
	window.setupView.browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	window.setupView.browseRightPrescription.callback(onBrowseRightPrescription, this);
	window.setupView.browseAudio.callback(onBrowseAudio, this);
	window.setupView.browseBrir.callback(onBrowseBrir, this);
	window.setupView.confirm.callback(onConfirmTestSetup, this);
	window.setupView.usingSpatialization_.callback(onToggleSpatialization, this);
	window.setupView.usingHearingAidSimulation_.callback(onToggleHearingAidSimulation, this);
	window.testerView.play.callback(onPlayTrial, this);
	window.calibrationView.play.callback(onPlayCalibration, this);
	window.calibrationView.stop.callback(onStopCalibration, this);
}

void FltkView::turnOnHearingAidSimulation() {
	window.setupView.usingHearingAidSimulation_.value(1);
}

void FltkView::turnOnSpatialization() {
	window.setupView.usingSpatialization_.value(1);
}

void FltkView::hideSubjectId()
{
	window.setupView.subjectId_.hide();
}

void FltkView::hideTesterId()
{
	window.setupView.testerId_.hide();
}

void FltkView::hideStimulusList()
{
	window.setupView.audioDirectory_.hide();
}

void FltkView::hideOutputFilePath()
{
	window.setupView.testFilePath_.hide();
}

void FltkView::hideConfirmButton()
{
	window.setupView.confirm.hide();
}

void FltkView::hideBrirFilePath()
{
	window.setupView.brirFilePath_.hide();
}

void FltkView::hideBrowseForBrirButton()
{
	window.setupView.browseBrir.hide();
}

void FltkView::hideUsingSpatializationCheckBox()
{
	window.setupView.usingSpatialization_.hide();
}

void FltkView::hideLeftDslPrescriptionFilePath()
{
	window.setupView.leftPrescriptionFilePath_.hide();
}

void FltkView::hideBrowseForLeftDslPrescriptionButton()
{
	window.setupView.browseLeftPrescription.hide();
}

void FltkView::hideRightDslPrescriptionFilePath()
{
	window.setupView.rightPrescriptionFilePath_.hide();
}

void FltkView::hideBrowseForRightDslPrescriptionButton()
{
	window.setupView.browseRightPrescription.hide();
}

void FltkView::hideAttack_ms()
{
	window.setupView.attack_ms_.hide();
}

void FltkView::hideRelease_ms()
{
	window.setupView.release_ms_.hide();
}

void FltkView::hideChunkSize()
{
	window.setupView.chunkSize_.hide();
}

void FltkView::hideWindowSize()
{
	window.setupView.windowSize_.hide();
}

void FltkView::hideUsingHearingAidSimulationCheckBox()
{
	window.setupView.usingHearingAidSimulation_.hide();
}

void FltkView::hideAudioFilePath()
{
	window.calibrationView.audioFilePath_.hide();
}

void FltkView::hidePlayButton()
{
	window.calibrationView.play.hide();
}

void FltkView::hideStopButton()
{
	window.calibrationView.stop.hide();
}

void FltkView::hideLevel_dB_Spl()
{
	window.testerView.level_dB_Spl_.hide();
}

void FltkView::showSubjectId()
{
	window.setupView.subjectId_.show();
}

void FltkView::showTesterId()
{
	window.setupView.testerId_.show();
}

void FltkView::showStimulusList()
{
	window.setupView.audioDirectory_.show();
}

void FltkView::showOutputFilePath()
{
	window.setupView.testFilePath_.show();
}

void FltkView::showConfirmButton()
{
	window.setupView.confirm.show();
}

void FltkView::showBrirFilePath()
{
	window.setupView.brirFilePath_.show();
}

void FltkView::showBrowseForBrirButton()
{
	window.setupView.browseBrir.show();
}

void FltkView::showUsingSpatializationCheckBox()
{
	window.setupView.usingSpatialization_.show();
}

void FltkView::showLeftDslPrescriptionFilePath()
{
	window.setupView.leftPrescriptionFilePath_.show();
}

void FltkView::showBrowseForLeftDslPrescriptionButton()
{
	window.setupView.browseLeftPrescription.show();
}

void FltkView::showRightDslPrescriptionFilePath()
{
	window.setupView.rightPrescriptionFilePath_.show();
}

void FltkView::showBrowseForRightDslPrescriptionButton()
{
	window.setupView.browseRightPrescription.show();
}

void FltkView::showAttack_ms()
{
	window.setupView.attack_ms_.show();
}

void FltkView::showRelease_ms()
{
	window.setupView.release_ms_.show();
}

void FltkView::showChunkSize()
{
	window.setupView.chunkSize_.show();
}

void FltkView::showWindowSize()
{
	window.setupView.windowSize_.show();
}

void FltkView::showUsingHearingAidSimulationCheckBox()
{
	window.setupView.usingHearingAidSimulation_.show();
}

void FltkView::showAudioFilePath()
{
	window.calibrationView.audioFilePath_.show();
}

void FltkView::showPlayButton()
{
	window.calibrationView.play.show();
}

void FltkView::showStopButton()
{
	window.calibrationView.stop.show();
}

void FltkView::showLevel_dB_Spl()
{
	window.testerView.level_dB_Spl_.show();
}

void FltkView::showTesterView() {
	window.testerView.show();
}

void FltkView::hideTesterView() {
	window.testerView.hide();
}

void FltkView::deactivateBrowseForBrirButton() {
	window.setupView.browseBrir.deactivate();
}

void FltkView::deactivateBrirFilePath() {
	window.setupView.brirFilePath_.deactivate();
}

void FltkView::activateBrowseForBrirButton() {
	window.setupView.browseBrir.activate();
}

void FltkView::activateBrirFilePath() {
	window.setupView.brirFilePath_.activate();
}

void FltkView::activateLeftDslPrescriptionFilePath() {
	window.setupView.leftPrescriptionFilePath_.activate();
}

void FltkView::activateRightDslPrescriptionFilePath() {
	window.setupView.rightPrescriptionFilePath_.activate();
}

void FltkView::activateBrowseForLeftDslPrescriptionButton() {
	window.setupView.browseLeftPrescription.activate();
}

void FltkView::activateBrowseForRightDslPrescriptionButton() {
	window.setupView.browseRightPrescription.activate();
}

void FltkView::deactivateLeftDslPrescriptionFilePath() {
	window.setupView.leftPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateRightDslPrescriptionFilePath() {
	window.setupView.rightPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateBrowseForLeftDslPrescriptionButton() {
	window.setupView.browseLeftPrescription.deactivate();
}

void FltkView::deactivateBrowseForRightDslPrescriptionButton() {
	window.setupView.browseRightPrescription.deactivate();
}

void FltkView::activateReleaseTime_ms() {
	window.setupView.release_ms_.activate();
}

void FltkView::activateAttackTime_ms() {
	window.setupView.attack_ms_.activate();
}

void FltkView::activateWindowSize() {
	window.setupView.windowSize_.activate();
}

void FltkView::activateChunkSize() {
	window.setupView.chunkSize_.activate();
}

void FltkView::deactivateReleaseTime_ms() {
	window.setupView.release_ms_.deactivate();
}

void FltkView::deactivateAttackTime_ms() {
	window.setupView.attack_ms_.deactivate();
}

void FltkView::deactivateWindowSize() {
	window.setupView.windowSize_.deactivate();
}

void FltkView::deactivateChunkSize() {
	window.setupView.chunkSize_.deactivate();
}

void FltkView::populateAudioDeviceMenu(std::vector<std::string> items) {
	window.testerView.audioDevice_.populate(std::move(items));
}

void FltkView::populateChunkSizeMenu(std::vector<std::string> items) {
	window.setupView.chunkSize_.populate(std::move(items));
}

void FltkView::populateWindowSizeMenu(std::vector<std::string> items) {
	window.setupView.windowSize_.populate(std::move(items));
}

std::string FltkView::audioDevice() {
	return window.testerView.audioDevice_.text();
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
	window.setupView.testFilePath_.value(p.c_str());
}

void FltkView::setAudioDirectory(std::string d) {
	window.setupView.audioDirectory_.value(d.c_str());
}

void FltkView::setLeftDslPrescriptionFilePath(std::string p) {
	window.setupView.leftPrescriptionFilePath_.value(p.c_str());
}

void FltkView::setRightDslPrescriptionFilePath(std::string p) {
	window.setupView.rightPrescriptionFilePath_.value(p.c_str());
}

void FltkView::setBrirFilePath(std::string p) {
	window.setupView.brirFilePath_.value(p.c_str());
}

std::string FltkView::subjectId() {
	return window.setupView.subjectId_.value();
}

std::string FltkView::testerId() {
	return window.setupView.testerId_.value();
}

std::string FltkView::testFilePath() {
	return window.setupView.testFilePath_.value();
}

std::string FltkView::audioFilePath() {
	return window.calibrationView.audioFilePath_.value();
}

std::string FltkView::audioDirectory() {
	return window.setupView.audioDirectory_.value();
}

std::string FltkView::leftDslPrescriptionFilePath() {
	return window.setupView.leftPrescriptionFilePath_.value();
}

std::string FltkView::rightDslPrescriptionFilePath() {
	return window.setupView.rightPrescriptionFilePath_.value();
}

std::string FltkView::brirFilePath() {
	return window.setupView.brirFilePath_.value();
}

std::string FltkView::level_dB_Spl() {
	return window.testerView.level_dB_Spl_.value();
}

std::string FltkView::attack_ms() {
	return window.setupView.attack_ms_.value();
}

std::string FltkView::release_ms() {
	return window.setupView.release_ms_.value();
}

std::string FltkView::windowSize() {
	return window.setupView.windowSize_.text();
}

std::string FltkView::chunkSize() {
	return window.setupView.chunkSize_.text();
}

bool FltkView::usingSpatialization() {
	return window.setupView.usingSpatialization_.value();
}

bool FltkView::usingHearingAidSimulation() {
	return window.setupView.usingHearingAidSimulation_.value();
}

void FltkView::showErrorDialog(std::string message) {
	fl_alert(message.c_str());
}
