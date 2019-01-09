#include "FltkWindow.h"
#include <FL/fl_ask.H>
#include <sstream>

void FltkWindow::onBrowseLeftPrescription(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->listener->browseForLeftDslPrescription();
}

void FltkWindow::onBrowseRightPrescription(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->listener->browseForRightDslPrescription();
}

void FltkWindow::onBrowseAudio(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->listener->browseForAudio();
}

void FltkWindow::onBrowseBrir(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->listener->browseForBrir();
}

void FltkWindow::onPlay(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->listener->play();
}

FltkWindow::FltkWindow() :
	window(800, 200, 600, 600),
	_leftPrescriptionFilePath(250, 50, 200, 45, "left DSL prescription file path"),
	_rightPrescriptionFilePath(250, 100, 200, 45, "right DSL prescription file path"),
	_audioDirectory(250, 150, 200, 45, "audio directory"),
	_brirFilePath(250, 200, 200, 45, "BRIR file path"),
	browseLeftPrescription(460, 50, 60, 45, "browse"),
	browseRightPrescription(460, 100, 60, 45, "browse"),
	browseAudio(460, 150, 60, 45, "browse"),
	browseBrir(460, 200, 60, 45, "browse"),
	_level_dB_Spl(250, 250, 200, 45, "level (dB SPL)"),
	_attack_ms(250, 300, 200, 45, "attack (ms)"),
	_release_ms(250, 350, 200, 45, "release (ms)"),
	_windowSize(250, 400, 200, 45, "window size (samples)"),
	_chunkSize(250, 450, 200, 45, "chunk size (samples)"),
	_audioDevice(250, 500, 200, 45, "audio device"),
	play(250, 550, 60, 45, "play")
{
	window.end();
	window.show();
	browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	browseRightPrescription.callback(onBrowseRightPrescription, this);
	browseAudio.callback(onBrowseAudio, this);
	browseBrir.callback(onBrowseBrir, this);
	play.callback(onPlay, this);
}

void FltkWindow::showTestSetup()
{
}

void FltkWindow::hideTestSetup()
{
}

void FltkWindow::showTesterView()
{
}

void FltkWindow::populateAudioDeviceMenu(std::vector<std::string> items) {
	for (const auto &s : items)
		_audioDevice.add(s.c_str());
	_audioDevice.value(0);
}

std::string FltkWindow::audioDevice() {
	return _audioDevice.text();
}

void FltkWindow::runEventLoop() {
	Fl::run();
}

void FltkWindow::subscribe(EventListener * listener) {
	this->listener = listener;
}

static std::string formatFilters(std::vector<std::string> filters) {
	std::stringstream stream;
	for (const auto &filter : filters)
		stream << filter << "\n";
	return stream.str();
}

std::string FltkWindow::browseForFile(std::vector<std::string> filters) {
	Fl_Native_File_Chooser chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
	chooser.filter(formatFilters(filters).c_str());
	browseResult = chooser.show();
	return chooser.filename();
}

enum class BrowseResult {
	cancelled = 1
};

bool FltkWindow::browseCancelled() {
	return browseResult == static_cast<int>(BrowseResult::cancelled);
}

std::string FltkWindow::browseForDirectory() {
	Fl_Native_File_Chooser chooser{};
	chooser.type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	browseResult = chooser.show();
	return chooser.filename();
}

void FltkWindow::setAudioDirectory(std::string d) {
	_audioDirectory.value(d.c_str());
}

void FltkWindow::setLeftDslPrescriptionFilePath(std::string p) {
	_leftPrescriptionFilePath.value(p.c_str());
}

void FltkWindow::setRightDslPrescriptionFilePath(std::string p) {
	_rightPrescriptionFilePath.value(p.c_str());
}

void FltkWindow::setBrirFilePath(std::string p) {
	_brirFilePath.value(p.c_str());
}

std::string FltkWindow::audioDirectory() {
	return _audioDirectory.value();
}

std::string FltkWindow::leftDslPrescriptionFilePath() {
	return _leftPrescriptionFilePath.value();
}

std::string FltkWindow::rightDslPrescriptionFilePath() {
	return _rightPrescriptionFilePath.value();
}

std::string FltkWindow::brirFilePath() {
	return _brirFilePath.value();
}

std::string FltkWindow::level_dB_Spl() {
	return _level_dB_Spl.value();
}

std::string FltkWindow::attack_ms() {
	return _attack_ms.value();
}

std::string FltkWindow::release_ms() {
	return _release_ms.value();
}

std::string FltkWindow::windowSize() {
	return _windowSize.value();
}

std::string FltkWindow::chunkSize() {
	return _chunkSize.value();
}

void FltkWindow::showErrorDialog(std::string message) {
	fl_alert(message.c_str());
}
