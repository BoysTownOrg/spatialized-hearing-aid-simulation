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

void FltkView::onPlay(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->playTrial();
}

void FltkView::onToggleSpatialization(Fl_Widget *, void *self) {
	static_cast<FltkView *>(self)->listener->toggleUsingSpatialization();
}

FltkSetupView::FltkSetupView(int x, int y, int w, int h, const char *) :
	Fl_Group{ x, y, w, h },
	testFilePath_(250, 50, 200, 45, "test file path"),
	browseTestFilePath(460, 50, 60, 45, "browse"),
	subjectId_(250, 100, 200, 45, "subject ID"),
	testerId_(250, 150, 200, 45, "tester ID"),
	leftPrescriptionFilePath_(250, 200, 200, 45, "left DSL prescription file path"),
	browseLeftPrescription(460, 200, 60, 45, "browse"),
	rightPrescriptionFilePath_(250, 250, 200, 45, "right DSL prescription file path"),
	browseRightPrescription(460, 250, 60, 45, "browse"),
	audioDirectory_(250, 300, 200, 45, "audio directory"),
	browseAudio(460, 300, 60, 45, "browse"),
	brirFilePath_(250, 350, 200, 45, "BRIR file path"),
	browseBrir(460, 350, 60, 45, "browse"),
	usingSpatialization_(100, 350, 60, 45),
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
	play(250, 550, 60, 45, "play trial")
{
	end();
}

FltkWindow::FltkWindow(int x, int y, int w, int h, const char *):
	Fl_Double_Window{ x, y, w, h },
	testerView{ 0, 0, 600, 700 },
	setupView{ 0, 0, 600, 700 }
{
	end();
}

FltkView::FltkView() :
	window{ 800, 200, 600, 700 }
{
	window.show();
	window.setupView.hide();
	window.testerView.hide();
	window.setupView.browseTestFilePath.callback(onBrowseTestFile, this);
	window.setupView.browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	window.setupView.browseRightPrescription.callback(onBrowseRightPrescription, this);
	window.setupView.browseAudio.callback(onBrowseAudio, this);
	window.setupView.browseBrir.callback(onBrowseBrir, this);
	window.setupView.confirm.callback(onConfirmTestSetup, this);
	window.setupView.usingSpatialization_.callback(onToggleSpatialization, this);
	window.testerView.play.callback(onPlay, this);
	window.setupView.usingSpatialization_.value(1);
}

void FltkView::showTestSetup() {
	window.setupView.show();
}

void FltkView::hideTestSetup() {
	window.setupView.hide();
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

static std::string formatFilters(std::vector<std::string> filters) {
	std::stringstream stream;
	for (auto filter : filters)
		stream << filter << "\n";
	return stream.str();
}

std::string FltkView::browseForOpeningFile(std::vector<std::string> filters) {
	Fl_Native_File_Chooser chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
	chooser.filter(formatFilters(std::move(filters)).c_str());
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
	Fl_Native_File_Chooser chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	browseResult = chooser.show();
	return chooser.filename();
}

std::string FltkView::browseForSavingFile(std::vector<std::string> filters) {
	Fl_Native_File_Chooser chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
	chooser.filter(formatFilters(std::move(filters)).c_str());
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

void FltkView::showErrorDialog(std::string message) {
	fl_alert(message.c_str());
}
