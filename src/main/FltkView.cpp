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

static void hideAllChildren(Fl_Group *parent) {
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

FltkTestSetupGroup::FltkTestSetupGroup(int x, int y, int w, int h, const char *):
	Fl_Group{ x, y, w, h },
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
	confirm{850, 550, 60, 25, "confirm" }
{
	end();
}

FltkWindow::FltkWindow(int x, int y, int w, int h, const char *):
	Fl_Double_Window{ x, y, w, h },
	testSetup{ 0, 0, w, h - 25 },
	audioDevice_{100, 550, 200, 25, "audio device" },
	playNextTrial{350, 550, 100, 25, "play next trial" }
{
	end();
}

FltkView::FltkView() :
	window{ 425, 300, 950, 600 },
	testSetup_{ &window.testSetup }
{
	Fl::set_font(0, "Segoe UI");
	window.color(FL_LIGHT2);
	window.testSetup.hearingAidSimulation.box(FL_ENGRAVED_BOX);
	window.testSetup.hearingAidSimulation.color(FL_LIGHT2);
	window.testSetup.spatialization.box(FL_ENGRAVED_BOX);
	window.testSetup.spatialization.color(FL_LIGHT2);
	window.testSetup.calibration.box(FL_ENGRAVED_BOX);
	window.testSetup.calibration.color(FL_LIGHT2);
	registerCallbacks();
	turnOnSpatialization();
	turnOnHearingAidSimulation();
	populateChunkSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.testSetup.hearingAidSimulation.chunkSize_.value(4);
	populateWindowSizeMenu({ "64", "128", "256", "512", "1024", "2048", "4096", "8192" });
	window.testSetup.hearingAidSimulation.windowSize_.value(2);
	window.testSetup.hearingAidSimulation.attack_ms_.value("5");
	window.testSetup.hearingAidSimulation.release_ms_.value("50");
	window.testSetup.calibration.level_dB_Spl_.value("65");
	hideAllChildren(&window);
	window.audioDevice_.show();
}

auto FltkView::testSetup() -> TestSetup * {
	return &testSetup_;
}

void FltkView::registerCallbacks() {
	window.testSetup.browseTestFilePath.callback(onBrowseTestFile, this);
	window.testSetup.hearingAidSimulation.browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	window.testSetup.hearingAidSimulation.browseRightPrescription.callback(onBrowseRightPrescription, this);
	window.testSetup.browseForStimulusList.callback(onBrowseStimulusList, this);
	window.testSetup.calibration.browseForAudioFile.callback(onBrowseAudio, this);
	window.testSetup.spatialization.browseBrir.callback(onBrowseBrir, this);
	window.testSetup.confirm.callback(onConfirmTestSetup, this);
	window.testSetup.usingSpatialization_.callback(onToggleSpatialization, this);
	window.testSetup.usingHearingAidSimulation_.callback(onToggleHearingAidSimulation, this);
	window.playNextTrial.callback(onPlayTrial, this);
	window.testSetup.calibration.play.callback(onPlayCalibration, this);
	window.testSetup.calibration.stop.callback(onStopCalibration, this);
	window.testSetup.calibration.save.callback(onSaveAudio, this);
}

void FltkView::turnOnHearingAidSimulation() {
	window.testSetup.usingHearingAidSimulation_.value(1);
}

void FltkView::turnOnSpatialization() {
	window.testSetup.usingSpatialization_.value(1);
}

void FltkView::hidePlayNextTrialButton()
{
	window.playNextTrial.hide();
}


void FltkView::showPlayNextTrialButton()
{
	window.playNextTrial.show();
}

void FltkView::deactivateBrowseForBrirButton() {
	window.testSetup.spatialization.browseBrir.deactivate();
}

void FltkView::deactivateBrirFilePath() {
	window.testSetup.spatialization.brirFilePath_.deactivate();
}

void FltkView::activateBrowseForBrirButton() {
	window.testSetup.spatialization.browseBrir.activate();
}

void FltkView::activateBrirFilePath() {
	window.testSetup.spatialization.brirFilePath_.activate();
}

void FltkView::activateLeftDslPrescriptionFilePath() {
	window.testSetup.hearingAidSimulation.leftPrescriptionFilePath_.activate();
}

void FltkView::activateRightDslPrescriptionFilePath() {
	window.testSetup.hearingAidSimulation.rightPrescriptionFilePath_.activate();
}

void FltkView::activateBrowseForLeftDslPrescriptionButton() {
	window.testSetup.hearingAidSimulation.browseLeftPrescription.activate();
}

void FltkView::activateBrowseForRightDslPrescriptionButton() {
	window.testSetup.hearingAidSimulation.browseRightPrescription.activate();
}

void FltkView::deactivateLeftDslPrescriptionFilePath() {
	window.testSetup.hearingAidSimulation.leftPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateRightDslPrescriptionFilePath() {
	window.testSetup.hearingAidSimulation.rightPrescriptionFilePath_.deactivate();
}

void FltkView::deactivateBrowseForLeftDslPrescriptionButton() {
	window.testSetup.hearingAidSimulation.browseLeftPrescription.deactivate();
}

void FltkView::deactivateBrowseForRightDslPrescriptionButton() {
	window.testSetup.hearingAidSimulation.browseRightPrescription.deactivate();
}

void FltkView::activateReleaseTime_ms() {
	window.testSetup.hearingAidSimulation.release_ms_.activate();
}

void FltkView::activateAttackTime_ms() {
	window.testSetup.hearingAidSimulation.attack_ms_.activate();
}

void FltkView::activateWindowSize() {
	window.testSetup.hearingAidSimulation.windowSize_.activate();
}

void FltkView::activateChunkSize() {
	window.testSetup.hearingAidSimulation.chunkSize_.activate();
}

void FltkView::deactivateReleaseTime_ms() {
	window.testSetup.hearingAidSimulation.release_ms_.deactivate();
}

void FltkView::deactivateAttackTime_ms() {
	window.testSetup.hearingAidSimulation.attack_ms_.deactivate();
}

void FltkView::deactivateWindowSize() {
	window.testSetup.hearingAidSimulation.windowSize_.deactivate();
}

void FltkView::deactivateChunkSize() {
	window.testSetup.hearingAidSimulation.chunkSize_.deactivate();
}

void FltkView::populateAudioDeviceMenu(std::vector<std::string> items) {
	window.audioDevice_.populate(std::move(items));
}

void FltkView::populateChunkSizeMenu(std::vector<std::string> items) {
	window.testSetup.hearingAidSimulation.chunkSize_.populate(std::move(items));
}

void FltkView::populateWindowSizeMenu(std::vector<std::string> items) {
	window.testSetup.hearingAidSimulation.windowSize_.populate(std::move(items));
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

bool FltkView::usingSpatialization() {
	return window.testSetup.usingSpatialization_.value();
}

bool FltkView::usingHearingAidSimulation() {
	return window.testSetup.usingHearingAidSimulation_.value();
}

void FltkView::showErrorDialog(std::string message) {
	fl_alert(message.c_str());
}

FltkView::FltkTestSetup::FltkTestSetup(FltkTestSetupGroup *view) : view{ view }
{
}

void FltkView::FltkTestSetup::setTestFilePath(std::string p) {
	view->testFilePath_.value(p.c_str());
}

void FltkView::FltkTestSetup::hide() {
	view->hide();
}

void FltkView::FltkTestSetup::show() {
	view->show();
}

void FltkView::FltkTestSetup::setStimulusList(std::string d) {
	view->stimulusList_.value(d.c_str());
}

void FltkView::FltkTestSetup::setLeftDslPrescriptionFilePath(std::string p) {
	view->hearingAidSimulation.leftPrescriptionFilePath_.value(p.c_str());
}

void FltkView::FltkTestSetup::setRightDslPrescriptionFilePath(std::string p) {
	view->hearingAidSimulation.rightPrescriptionFilePath_.value(p.c_str());
}

void FltkView::FltkTestSetup::setBrirFilePath(std::string p) {
	view->spatialization.brirFilePath_.value(p.c_str());
}

void FltkView::FltkTestSetup::setAudioFilePath(std::string p)
{
	view->calibration.audioFilePath_.value(p.c_str());
}

std::string FltkView::FltkTestSetup::subjectId() {
	return view->subjectId_.value();
}

std::string FltkView::FltkTestSetup::testerId() {
	return view->testerId_.value();
}

std::string FltkView::FltkTestSetup::testFilePath() {
	return view->testFilePath_.value();
}

std::string FltkView::FltkTestSetup::audioFilePath() {
	return view->calibration.audioFilePath_.value();
}

std::string FltkView::FltkTestSetup::stimulusList() {
	return view->stimulusList_.value();
}

std::string FltkView::FltkTestSetup::leftDslPrescriptionFilePath() {
	return view->hearingAidSimulation.leftPrescriptionFilePath_.value();
}

std::string FltkView::FltkTestSetup::rightDslPrescriptionFilePath() {
	return view->hearingAidSimulation.rightPrescriptionFilePath_.value();
}

std::string FltkView::FltkTestSetup::brirFilePath() {
	return view->spatialization.brirFilePath_.value();
}

std::string FltkView::FltkTestSetup::level_dB_Spl() {
	return view->calibration.level_dB_Spl_.value();
}

std::string FltkView::FltkTestSetup::attack_ms() {
	return view->hearingAidSimulation.attack_ms_.value();
}

std::string FltkView::FltkTestSetup::release_ms() {
	return view->hearingAidSimulation.release_ms_.value();
}

std::string FltkView::FltkTestSetup::windowSize() {
	return view->hearingAidSimulation.windowSize_.text();
}

std::string FltkView::FltkTestSetup::chunkSize() {
	return view->hearingAidSimulation.chunkSize_.text();
}
