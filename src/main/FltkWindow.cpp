#include "FltkWindow.h"
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

FltkWindow::FltkWindow() :
	window(500, 300, 450, 500),
	_leftPrescriptionFilePath(150, 50, 200, 40, "Left DSL prescription file path"),
	_rightPrescriptionFilePath(150, 100, 200, 40, "Right DSL prescription file path"),
	_audioFilePath(150, 150, 200, 40, "Audio file path"),
	_brirFilePath(150, 200, 200, 40, "BRIR file path"),
	browseLeftPrescription(360, 50, 60, 40, "Browse"),
	browseRightPrescription(360, 100, 60, 40, "Browse"),
	browseAudio(360, 150, 60, 40, "Browse"),
	browseBrir(360, 200, 60, 40, "Browse"),
	_level_dB_Spl(150, 250, 200, 40, "Level (dB SPL)"),
	_attack_ms(150, 300, 200, 40, "attack (ms)"),
	_release_ms(150, 350, 200, 40, "release (ms)"),
	_windowSize(150, 400, 200, 40, "window size (samples)"),
	_chunkSize(150, 450, 200, 40, "chunk size (samples)")
{
	window.end();
	window.show();
	browseLeftPrescription.callback(onBrowseLeftPrescription, this);
	browseRightPrescription.callback(onBrowseRightPrescription, this);
	browseAudio.callback(onBrowseAudio, this);
	browseBrir.callback(onBrowseBrir, this);
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
