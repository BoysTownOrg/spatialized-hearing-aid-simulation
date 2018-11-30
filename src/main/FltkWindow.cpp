#include "FltkWindow.h"
#include <FL/fl_ask.H>
#include <sstream>

void FltkWindow::onBrowseLeftPrescription(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForLeftDslPrescription();
}

void FltkWindow::onBrowseRightPrescription(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForRightDslPrescription();
}

void FltkWindow::onBrowseAudio(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForAudio();
}

void FltkWindow::onBrowseBrir(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForBrir();
}

void FltkWindow::onPlay(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->play();
}

FltkWindow::FltkWindow() :
	window(800, 200, 600, 550),
	_leftPrescriptionFilePath(250, 50, 200, 45, "left DSL prescription file path"),
	_rightPrescriptionFilePath(250, 100, 200, 45, "right DSL prescription file path"),
	_audioFilePath(250, 150, 200, 45, "audio file path"),
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
	play(250, 500, 60, 45, "play")
{
	window.end();
	window.show();
	browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	browseRightPrescription.callback(onBrowseRightPrescription, this);
	browseAudio.callback(onBrowseAudio, this);
	browseBrir.callback(onBrowseBrir, this);
	play.callback(onPlay, this);
}

void FltkWindow::runEventLoop() {
	Fl::run();
}

void FltkWindow::setPresenter(SpatializedHearingAidSimulationPresenter *c) {
	presenter = c;
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

void FltkWindow::setLeftDslPrescriptionFilePath(std::string p) {
	_leftPrescriptionFilePath.value(p.c_str());
}

void FltkWindow::setRightDslPrescriptionFilePath(std::string p) {
	_rightPrescriptionFilePath.value(p.c_str());
}

void FltkWindow::setAudioFilePath(std::string p) {
	_audioFilePath.value(p.c_str());
}

void FltkWindow::setBrirFilePath(std::string p) {
	_brirFilePath.value(p.c_str());
}

std::string FltkWindow::leftDslPrescriptionFilePath() const {
	return _leftPrescriptionFilePath.value();
}

std::string FltkWindow::rightDslPrescriptionFilePath() const {
	return _rightPrescriptionFilePath.value();
}

std::string FltkWindow::audioFilePath() const {
	return _audioFilePath.value();
}

std::string FltkWindow::brirFilePath() const {
	return _brirFilePath.value();
}

std::string FltkWindow::level_dB_Spl() const {
	return _level_dB_Spl.value();
}

std::string FltkWindow::attack_ms() const {
	return _attack_ms.value();
}

std::string FltkWindow::release_ms() const {
	return _release_ms.value();
}

std::string FltkWindow::windowSize() const {
	return _windowSize.value();
}

std::string FltkWindow::chunkSize() const {
	return _chunkSize.value();
}

void FltkWindow::showErrorDialog(std::string message) {
	fl_alert(message.c_str());
}
