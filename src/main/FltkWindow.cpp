#include "FltkWindow.h"
#include <sstream>

void FltkWindow::onBrowsePrescription(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForLeftDslPrescription();
}

void FltkWindow::onBrowseAudio(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForAudio();
}

void FltkWindow::onBrowseBrir(Fl_Widget *, void *self) {
	static_cast<FltkWindow *>(self)->presenter->browseForBrir();
}

FltkWindow::FltkWindow() :
	window(500, 300, 450, 300),
	_prescriptionFilePath(150, 50, 200, 40, "Left DSL prescription file path"),
	_audioFilePath(150, 100, 200, 40, "Audio file path"),
	_brirFilePath(150, 150, 200, 40, "BRIR file path"),
	browsePrescription(360, 50, 60, 40, "Browse"),
	browseAudio(360, 100, 60, 40, "Browse"),
	browseBrir(360, 150, 60, 40, "Browse")
{
	window.end();
	window.show();
	browsePrescription.callback(onBrowsePrescription, this);
	browseAudio.callback(onBrowseAudio, this);
	browseBrir.callback(onBrowseBrir, this);
}

void FltkWindow::runEventLoop() {
	Fl::run();
}

void FltkWindow::setPresenter(Presenter *c) {
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
	_prescriptionFilePath.value(p.c_str());
}

void FltkWindow::setAudioFilePath(std::string p) {
	_audioFilePath.value(p.c_str());
}

void FltkWindow::setBrirFilePath(std::string p) {
	_brirFilePath.value(p.c_str());
}

std::string FltkWindow::leftDslPrescriptionFilePath() const {
	return _prescriptionFilePath.value();
}

std::string FltkWindow::audioFilePath() const {
	return _audioFilePath.value();
}

std::string FltkWindow::brirFilePath() const {
	return _brirFilePath.value();
}
